#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include <stdexcept>
#include <algorithm>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Fabric/CurrentExecuters.h"

class ExecuterQueueRHI {
private:
    std::vector<FabricRHI::ExecuteImplVariant*> executers;
    std::mutex executersMutex;
    
    std::atomic<bool> isExecuting{false};
    std::shared_ptr<BaseDevice> device;

    bool needSort{false};

    bool InternalExecute(bool throwException) {
        bool expected = false;
        if (!isExecuting.compare_exchange_strong(expected, true)) {
            if (throwException) {
                throw std::runtime_error("Error in Execute: already executing!");
            }
            return false;
        }
        
        try {
            if (!device) throw std::runtime_error("Device is dead");
            
            std::lock_guard<std::mutex> lock(executersMutex);
            
            if (needSort) {
                std::sort(executers.begin(), executers.end(),
                    [](FabricRHI::ExecuteImplVariant* a, FabricRHI::ExecuteImplVariant* b) {
                        return a->GetLayer() < b->GetLayer();
                    });
            }
            
            for (auto* exec : executers) {
                if (!exec) continue;
                if (exec->IsMarkedToProcess()) {
                    exec->ProcessParams(*device);
                    exec->MarkProcess(false);
                }
            }
        }
        catch (...) {
            isExecuting = false;
            if (throwException) {
                throw;
            }
            return false;
        }
    
        isExecuting = false;
        return true;
    }

public:
    ExecuterQueueRHI(std::shared_ptr<BaseDevice> dev)
        : device(dev) {
        executers.reserve(64);
    }
    
    ~ExecuterQueueRHI() {
        Clear();
    }

    void AddExecuter(FabricRHI::ExecuteImplVariant* exec) {
        if (!exec) return;
        std::lock_guard<std::mutex> lock(executersMutex);
        executers.push_back(exec);
        needSort = true;
    }

    template<typename T, typename... Args>
    T* CreateAndAdd(Args&&... args) {
        auto* exec = new T(std::forward<Args>(args)...);
        AddExecuter(exec);
        return exec;
    }

    void MarkAllForProcessing() {
        std::lock_guard<std::mutex> lock(executersMutex);
        for (auto* exec : executers) {
            if (exec) exec->MarkProcess(true);
        }
    }

    void MarkForProcessing(FabricRHI::ExecuteImplVariant* exec) {
        if (!exec) return;
        exec->MarkProcess(true);
    }

    void MarkForProcessing(size_t index) {
        std::lock_guard<std::mutex> lock(executersMutex);
        if (index < executers.size() && executers[index]) {
            executers[index]->MarkProcess(true);
        }
    }

    void RemoveExecuter(FabricRHI::ExecuteImplVariant* exec) {
        if (!exec) return;
        std::lock_guard<std::mutex> lock(executersMutex);
        auto it = std::find(executers.begin(), executers.end(), exec);
        if (it != executers.end()) {
            executers.erase(it);
        }
        needSort = true;
    }

    void RemoveExecuter(size_t index) {
        std::lock_guard<std::mutex> lock(executersMutex);
        if (index < executers.size()) {
            executers.erase(executers.begin() + index);
        }
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(executersMutex);
        executers.clear();
    }

    void Execute() {
        try {
            InternalExecute(true);
        }
        catch(...) {
            throw;
        }
    }

    bool TryExecute() {
        return InternalExecute(false);
    }

    bool IsExecuting() const {
        return isExecuting.load();
    }

    void Present() {
        if (!device) throw std::runtime_error("Device is dead");
        device->Present();
    }

    size_t GetExecuterCount() const {
        std::lock_guard<std::mutex> lock(executersMutex);
        return executers.size();
    }

    bool HasExecuters() const {
        std::lock_guard<std::mutex> lock(executersMutex);
        return !executers.empty();
    }

    FabricRHI::ExecuteImplVariant* GetExecuter(size_t index) {
        std::lock_guard<std::mutex> lock(executersMutex);
        if (index < executers.size()) {
            return executers[index];
        }
        return nullptr;
    }

    const FabricRHI::ExecuteImplVariant* GetExecuter(size_t index) const {
        std::lock_guard<std::mutex> lock(executersMutex);
        if (index < executers.size()) {
            return executers[index];
        }
        return nullptr;
    }

    void ClearAllMarks() {
        std::lock_guard<std::mutex> lock(executersMutex);
        for (auto* exec : executers) {
            if (exec) exec->MarkProcess(false);
        }
    }

    bool HasMarkedExecuters() const {
        std::lock_guard<std::mutex> lock(executersMutex);
        return std::any_of(executers.begin(), executers.end(),
            [](FabricRHI::ExecuteImplVariant* exec) {
                return exec && exec->IsMarkedToProcess();
            });
    }

    size_t GetMarkedExecuterCount() const {
        std::lock_guard<std::mutex> lock(executersMutex);
        return std::count_if(executers.begin(), executers.end(),
            [](FabricRHI::ExecuteImplVariant* exec) {
                return exec && exec->IsMarkedToProcess();
            });
    }
};