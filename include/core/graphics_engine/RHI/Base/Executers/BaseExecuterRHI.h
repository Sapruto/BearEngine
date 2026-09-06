#pragma once

#include <concepts>
#include <vector>
#include <algorithm>
#include <optional>
#include <functional>
#include <variant>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include <atomic>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"

template<typename T>
concept HasProcessParamsImpl = requires(T* t, BaseDevice& device) {
    { t->ProcessParamsImpl(device) } -> std::same_as<void>;
};

struct BaseProcessResult {
    int paramID;
    bool success{true};
    std::string errorMessage;
};

using SubscriptionId = uint64_t;

struct SubscriptionToken {
    SubscriptionId id;
    int paramID;
    
    bool operator==(const SubscriptionToken& other) const {
        return id == other.id && paramID == other.paramID;
    }
    
    bool operator!=(const SubscriptionToken& other) const {
        return !(*this == other);
    }
};

template<typename ExecuterImplRHI, typename Params, typename ProcessResult>
    requires(std::is_variant_v<Params> && 
            HasProcessParamsImpl<ExecuterImplRHI> && 
            std::is_base_of_v<BaseProcessResult, ProcessResult>)
class BaseExecuterRHI {
private:
    SubscriptionToken SubscribeInternal(int paramID, CallbackType callback, bool isOnce) {
        SubscriptionId id = nextId.fetch_add(1);
        subscribers.emplace_back(id, paramID, std::move(callback), isOnce);
        return {id, paramID};
    }

    void UpdateSubscriptionsAfterRemoval(int removedIndex) {
        for (auto& sub : subscribers) {
            if (sub.paramID > removedIndex) {
                sub.paramID--;
            }
        }
    }

protected:
    using ParamsType = Params;
    using ResultType = ProcessResult;
    using CallbackType = std::function<void(const std::vector<ProcessResult>&)>;
    using OnceCallbackType = std::function<void(const std::vector<ProcessResult>&)>;

    struct Subscriber {
        SubscriptionId id;
        int paramID;
        CallbackType callback;
        bool isOnce{false};
        
        Subscriber(SubscriptionId id, int paramID, CallbackType callback, bool isOnce = false)
            : id(id), paramID(paramID), callback(std::move(callback)), isOnce(isOnce) {}
    };

    ExecuterTypeRHI type;
    unsigned int layer{0};
    std::vector<Params> params;
    
    std::vector<Subscriber> subscribers;
    std::atomic<SubscriptionId> nextId{1};

    bool markProcess{false};

    void NotifySubscribers(const std::vector<ProcessResult>& results) {
        for (const auto& subscriber : subscribers) {
            std::vector<ProcessResult> filteredResults;
            
            if (subscriber.paramID == -1) {
                filteredResults = results;
            } else {
                for (const auto& result : results) {
                    if (result.paramID == subscriber.paramID) {
                        filteredResults.push_back(result);
                    }
                }
            }
            
            if (!filteredResults.empty()) {
                subscriber.callback(filteredResults);
            }
        }
        
        RemoveOnceSubscribers();
    }

    void RemoveOnceSubscribers() {
        subscribers.erase(
            std::remove_if(subscribers.begin(), subscribers.end(),
                [](const Subscriber& sub) { return sub.isOnce; }),
            subscribers.end()
        );
    }

public:
    BaseExecuterRHI() = default;
    ~BaseExecuterRHI() = default;

    void ProcessParams(BaseDevice& device) {
        static_cast<ExecuterImplRHI*>(this)->ProcessParamsImpl(device);
    }

    SubscriptionToken Subscribe(int paramID, CallbackType callback) {
        return SubscribeInternal(paramID, std::move(callback), false);
    }

    SubscriptionToken SubscribeOnce(int paramID, CallbackType callback) {
        return SubscribeInternal(paramID, std::move(callback), true);
    }

    SubscriptionToken SubscribeAll(CallbackType callback) {
        return SubscribeInternal(-1, std::move(callback), false);
    }

    SubscriptionToken SubscribeAllOnce(CallbackType callback) {
        return SubscribeInternal(-1, std::move(callback), true);
    }

    bool Unsubscribe(const SubscriptionToken& token) {
        auto it = std::find_if(subscribers.begin(), subscribers.end(),
            [&token](const Subscriber& sub) {
                return sub.id == token.id && sub.paramID == token.paramID;
            });
        
        if (it != subscribers.end()) {
            subscribers.erase(it);
            return true;
        }
        return false;
    }

    size_t UnsubscribeAll(int paramID) {
        auto newEnd = std::remove_if(subscribers.begin(), subscribers.end(),
            [paramID](const Subscriber& sub) {
                return sub.paramID == paramID;
            });
        
        size_t removed = std::distance(newEnd, subscribers.end());
        subscribers.erase(newEnd, subscribers.end());
        return removed;
    }

    void UnsubscribeAll() {
        subscribers.clear();
    }

    bool HasSubscribers() const {
        return !subscribers.empty();
    }

    bool HasSubscribers(int paramID) const {
        return std::any_of(subscribers.begin(), subscribers.end(),
            [paramID](const Subscriber& sub) {
                return sub.paramID == paramID || sub.paramID == -1;
            });
    }

    size_t GetSubscriberCount() const {
        return subscribers.size();
    }

    ExecuterTypeRHI GetExecuterType() const { return type; }
    int GetLayer() const { return layer; }
    const std::vector<Params>& GetParams() const { return params; }
    std::vector<Params>& GetParams() { return params; }

    void SetLayer(unsigned int newLayer) { layer = newLayer; }

    int AddParam(Params param) {
        int id = static_cast<int>(params.size());
        params.push_back(std::move(param));
        return id;
    }

    template<typename... Args>
    int EmplaceParam(Args&&... args) {
        int id = static_cast<int>(params.size());
        params.emplace_back(std::forward<Args>(args)...);
        return id;
    }

    void RemoveParam(size_t index) {
        if (index < params.size()) {
            params.erase(params.begin() + index);
            UpdateSubscriptionsAfterRemoval(static_cast<int>(index));
        }
    }

    void ClearParams() {
        params.clear();
        auto newEnd = std::remove_if(subscribers.begin(), subscribers.end(),
            [](const Subscriber& sub) { return sub.paramID != -1; });
        subscribers.erase(newEnd, subscribers.end());
    }

    void ReserveParams(size_t count) {
        params.reserve(count);
    }

    size_t GetParamCount() const { return params.size(); }
    bool HasParams() const { return !params.empty(); }

    Params& GetParam(size_t index) {
        if (index >= params.size()) {
            throw std::out_of_range("Index out of range");
        }
        return params[index];
    }

    const Params& GetParam(size_t index) const {
        if (index >= params.size()) {
            throw std::out_of_range("Index out of range");
        }
        return params[index];
    }

    template<typename T>
    std::optional<T> GetParamByType() const {
        for (const auto& param : params) {
            if (std::holds_alternative<T>(param)) {
                return std::get<T>(param);
            }
        }
        return std::nullopt;
    }

    void Reset() {
        params.clear();
        UnsubscribeAll();
        layer = 0;
    }

    void MarkProcess(bool newValue) {
        markProcess = newValue;
    }
    bool IsMarkedToProcess() const { return markProcess; }

    bool operator==(const BaseExecuterRHI& other) const {
        return this == &other;
    }

    bool operator!=(const BaseExecuterRHI& other) const {
        return !(*this == other);
    }
};