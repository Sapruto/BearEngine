#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include <stdexcept>

#include "RHI/Commands/BaseCommand.h"

template<typename CommandBufferImplRHI>
class BaseCommandBufferRHI {
private:
    struct CommandBuffer {
        std::vector<BaseCommand> cmds;
        bool ready = false;

        bool Empty() const {
            return cmds.empty();
        }
        void Clear() {
            cmds.clear();
            ready = false;
        }
    };
    
    std::mutex bufferPoolMutex;
    std::vector<std::unique_ptr<CommandBuffer>> freeBuffers;
    std::vector<std::unique_ptr<CommandBuffer>> pendingBuffers;
    
    std::mutex currentBufferMutex;
    std::unique_ptr<CommandBuffer> currentWriteBuffer;

    std::atomic<bool> isExecuting{false};
    std::atomic<bool> shutdownRequested{false};

    bool InternalExecute(bool throwExeption) {
        RenderImpl* renderImpl = static_cast<CommandBufferImplRHI*>(this);

        bool expected = false;
        if (!isExecuting.compare_exchange_strong(expected, true)) {
            if (throwExeption) {
                throw std::runtime_error("Error in Execute: already executing!");
            }
            else {
                return false;
            }
        }
        
        try {
            std::vector<std::unique_ptr<CommandBuffer>> buffersToExecute;
            
            {
                std::lock_guard<std::mutex> lock(bufferPoolMutex);
                buffersToExecute.swap(pendingBuffers);
            }
            
            for (auto& buffer : buffersToExecute) {
                if (buffer && buffer->ready) {
                    renderImpl->ExecuteImpl(*buffer);
                    buffer->ready = false;
                    
                    std::lock_guard<std::mutex> lock(bufferPoolMutex);
                    freeBuffers.push_back(std::move(buffer));
                }
            }
        }
        catch (...) {
            isExecuting = false;
            if (throwExeption) {
                throw std::runtime_error("Error in Execute: some buffer is not processed by RenderImpl.");
            }
            return false;
        }
    
        isExecuting = false;
        return true;
    }

    void SortBuffers() {
        std::sort(currentWriteBuffer->cmds.begin(), currentWriteBuffer->cmds.end(),
            [](const BaseCommand& a, const BaseCommand& b) {
                return a.GetLayer() < b.GetLayer();
            });
    }
    
    template<typename T>
    concept HasExecuteImpl = requires(T* t, const CommandBuffer& buf) {
        { t->ExecuteImpl(buf) } -> std::same_as<void>;
    };
    
    template<typename T>
    concept HasInitImpl = requires(T* t) {
        { t->InitImpl() } -> std::same_as<void>;
    };
    
    template<typename T>
    concept HasShutdownImpl = requires(T* t) {
        { t->ShutdownImpl() } -> std::same_as<void>;
    };

public:
    BaseCommandBufferRHI(int defaultBuffers = 3) {
        static_assert(HasExecuteImpl<RenderImpl>, 
            "RenderImpl must implement ExecuteImpl(const CommandBuffer&)");
        static_assert(HasInitImpl<RenderImpl>, 
            "RenderImpl must implement InitImpl()");
        static_assert(HasShutdownImpl<RenderImpl>, 
            "RenderImpl must implement ShutdownImpl()");

        for (int i = 0; i < defaultBuffers; ++i) {
            freeBuffers.push_back(std::make_unique<CommandBuffer>());
        }
        currentWriteBuffer = std::make_unique<CommandBuffer>();
    }

    void RecordCmd(const BaseCommand& cmd) {
        std::lock_guard<std::mutex> lock(currentBufferMutex);
        if (shutdownRequested.load()) {
            throw std::runtime_error("BaseCommandBufferRHI: shutdown in progress!");
        }
        currentWriteBuffer->cmds.push_back(cmd);
        SortBuffers();
    }

    void Flush() {
        if (shutdownRequested.load()) {
            return;
        }
        if (!currentWriteBuffer) return;
        if (currentWriteBuffer->Empty()) return;
        
        std::lock_guard<std::mutex> lock(bufferPoolMutex);
        
        std::unique_ptr<CommandBuffer> newBuffer;
        if (!freeBuffers.empty()) {
            newBuffer = std::move(freeBuffers.back());
            freeBuffers.pop_back();
            newBuffer->Clear();
        } 
        else {
            newBuffer = std::make_unique<CommandBuffer>();
        }
        
        currentWriteBuffer->ready = true;
        pendingBuffers.push_back(std::move(currentWriteBuffer));
        
        currentWriteBuffer = std::move(newBuffer);
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
    
    void Init() {
        RenderImpl* renderImpl = static_cast<CommandBufferImplRHI*>(this);
        renderImpl->InitImpl();
    }
    void Shutdown() {
        shutdownRequested = true;
        RenderImpl* renderImpl = static_cast<CommandBufferImplRHI*>(this);
        renderImpl->ShutdownImpl();
    }

    bool IsExecuting() const {
        return isExecuting.load();
    }

    bool HasPendingCommands() const {
        std::lock_guard<std::mutex> lock(bufferPoolMutex);
        return !pendingBuffers.empty();
    }

    void Clear() {
        std::scoped_lock lock(bufferPoolMutex, currentBufferMutex);
        for (auto& buffer : pendingBuffers) {
            buffer->Clear();
            freeBuffers.push_back(std::move(buffer));
        }
        pendingBuffers.clear();
        currentWriteBuffer->Clear();
    }
};