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
#include <stdexcept>
#include <string>
#include <chrono>
#include <limits>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"
#include "RHI/Base/Executors/IExecutorRHI.h"

namespace RHI::Executors {
    template<typename T>
    concept HasProcessCurrentParam = requires(T* t, RHI::Base::BaseDevice& device,
                                            const typename T::ParamsType& p) {
        { t->ProcessCurrentParam(device, p) } -> std::same_as<typename T::ResultType>;
    };

    inline constexpr uint64_t kAnyParamId = std::numeric_limits<uint64_t>::max();

    struct BaseProcessResult {
        uint64_t paramId = kAnyParamId;
        int layer = -1;
        bool success{true};
        std::string errorMessage;

        BaseProcessResult() = default;
        BaseProcessResult(uint64_t id, bool ok = true, std::string msg = {})
            : paramId(id)
            , success(ok)
            , errorMessage(std::move(msg)) {}
        BaseProcessResult(uint64_t id, int layer, bool ok = true, std::string msg = {})
            : paramId(id)
            , layer(layer)
            , success(ok)
            , errorMessage(std::move(msg)) {}
    };

    struct SubscriptionToken {
        uint64_t id{0};
        uint64_t paramId{kAnyParamId};

        bool operator==(const SubscriptionToken& other) const noexcept {
            return id == other.id && paramId == other.paramId;
        }
        bool operator!=(const SubscriptionToken& other) const noexcept {
            return !(*this == other);
        }
        bool IsValid() const noexcept { return id != 0; }
    };

    template<typename Params>
    struct ParamToken {
        uint64_t id;
        int layer;
        Params params;
        bool removeAfterExecute;
        bool enabled;

        ParamToken(uint64_t id, int layer, Params params,
                bool removeAfterExecute = false, bool enabled = true)
            : id(id)
            , layer(layer)
            , params(std::move(params))
            , removeAfterExecute(removeAfterExecute)
            , enabled(enabled) {}

        bool operator==(const ParamToken& other) const noexcept {
            return id == other.id;
        }
        bool operator!=(const ParamToken& other) const noexcept {
            return id != other.id;
        }
    };

    template<typename T>
    struct is_variant : std::false_type {};
    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};
    template<typename T>
    inline constexpr bool is_variant_v = is_variant<T>::value;

    template<typename ExecutorImplRHI, typename Params, typename ProcessResult>
        requires(is_variant_v<Params> &&
                std::is_base_of_v<BaseProcessResult, ProcessResult>)
    class BaseExecutorRHI : public IExecutorRHI {
    public:
        using ParamsType = Params;
        using ResultType = ProcessResult;
        using ParamTokenT = ParamToken<Params>;
        using CallbackType = std::function<void(const std::vector<ProcessResult>&)>;
        using OnceCallbackType = CallbackType;

    protected:
        struct Subscriber {
            uint64_t id;
            uint64_t paramId;
            int layerFilter;
            CallbackType callback;
            bool isOnce{false};
            bool enabled{true};

            Subscriber(uint64_t id, uint64_t paramId, int layerFilter,
                    CallbackType cb, bool isOnce = false)
                : id(id)
                , paramId(paramId)
                , layerFilter(layerFilter)
                , callback(std::move(cb))
                , isOnce(isOnce) {}
        };

        ExecutorTypeRHI type{ExecutorTypeRHI::Base};
        unsigned int layer{0};

        std::vector<ParamTokenT> params;
        std::vector<Subscriber> subscribers;

        bool markProcess{false};

    private:
        std::atomic<uint64_t> nextParamId{1};
        std::atomic<uint64_t> nextSubscriptionId{1};

        SubscriptionToken SubscribeInternal(uint64_t paramId, int layerFilter, CallbackType callback, bool isOnce) {
            const uint64_t id = nextSubscriptionId.fetch_add(1);
            subscribers.emplace_back(id, paramId, layerFilter, std::move(callback), isOnce);
            return SubscriptionToken{id, paramId};
        }

        void PruneAutoRemoveParams() {
            params.erase(
                std::remove_if(params.begin(), params.end(),
                    [](const ParamTokenT& p) { return p.removeAfterExecute; }),
                params.end());
        }

        void NotifySubscribers(const std::vector<ProcessResult>& results) {
            if (subscribers.empty()) return;

            std::vector<size_t> toRemove;

            for (size_t i = 0; i < subscribers.size(); i++) {
                auto& sub = subscribers[i];
                if (!sub.enabled) continue;

                if (sub.paramId == kAnyParamId && sub.layerFilter == -1) {
                    sub.callback(results);
                }
                else {
                    std::vector<ProcessResult> filtered;
                    filtered.reserve(results.size());
                    for (const auto& r : results) {
                        if (sub.paramId != kAnyParamId && r.paramId != sub.paramId) continue;
                        if (sub.layerFilter != -1 && r.layer != sub.layerFilter) continue;
                        filtered.push_back(r);
                    }
                    if (filtered.empty()) continue;
                    sub.callback(filtered);
                }

                if (sub.isOnce) toRemove.push_back(i);
            }

            for (auto it = toRemove.rbegin(); it != toRemove.rend(); it++) {
                subscribers.erase(subscribers.begin() + *it);
            }
        }

        bool ShouldProcessParam(const ParamTokenT& token) const {
            return token.enabled;
        }

        ProcessResult ProcessSingleParam(RHI::Base::BaseDevice& device, const Params& p) {
            return impl()->ProcessCurrentParam(device, p);
        }

        ExecutorImplRHI* impl() noexcept { return static_cast<ExecutorImplRHI*>(this); }
        const ExecutorImplRHI* impl() const noexcept { return static_cast<const ExecutorImplRHI*>(this); }

    public:
        BaseExecutorRHI() {
            static_assert(HasProcessCurrentParam<ExecutorImplRHI>,
                        "ExecutorImplRHI must implement ProcessCurrentParam(device, Params)");
        }
        ~BaseExecutorRHI() override = default;

        void ProcessParams(RHI::Base::BaseDevice& device) override {
            if (!markProcess) return;
            if (!impl()->IsValid()) return;

            std::vector<ProcessResult> results;
            results.reserve(params.size());

            for (size_t i = 0; i < params.size(); i++) {
                auto& token = params[i];
                if (!ShouldProcessParam(token)) continue;

                ProcessResult r = ProcessSingleParam(device, token.params);
                r.paramId = token.id;
                r.layer = token.layer;
                results.push_back(std::move(r));
            }

            NotifySubscribers(results);
            PruneAutoRemoveParams();
            markProcess = false;
        }

        bool IsValid() override {
            return impl()->IsValid();
        }

        SubscriptionToken Subscribe(uint64_t paramId, CallbackType cb) {
            return SubscribeInternal(paramId, -1, std::move(cb), false);
        }
        SubscriptionToken SubscribeOnce(uint64_t paramId, CallbackType cb) {
            return SubscribeInternal(paramId, -1, std::move(cb), true);
        }
        SubscriptionToken SubscribeAll(CallbackType cb) {
            return SubscribeInternal(kAnyParamId, -1, std::move(cb), false);
        }
        SubscriptionToken SubscribeAllOnce(CallbackType cb) {
            return SubscribeInternal(kAnyParamId, -1, std::move(cb), true);
        }

        SubscriptionToken SubscribeOnLayer(uint64_t paramId, int layer, CallbackType cb) {
            return SubscribeInternal(paramId, layer, std::move(cb), false);
        }
        SubscriptionToken SubscribeAllOnLayer(int layer, CallbackType cb) {
            return SubscribeInternal(kAnyParamId, layer, std::move(cb), false);
        }

        bool SetSubscriptionEnabled(const SubscriptionToken& token, bool enabled) {
            auto it = std::find_if(subscribers.begin(), subscribers.end(),
                [&](const Subscriber& s) { return s.id == token.id && s.paramId == token.paramId; });
            if (it == subscribers.end()) return false;
            it->enabled = enabled;
            return true;
        }

        bool Unsubscribe(const SubscriptionToken& token) {
            auto it = std::find_if(subscribers.begin(), subscribers.end(),
                [&](const Subscriber& s) {
                    return s.id == token.id && s.paramId == token.paramId;
                });
            if (it == subscribers.end()) return false;
            subscribers.erase(it);
            return true;
        }

        size_t UnsubscribeAll(uint64_t paramId) {
            auto newEnd = std::remove_if(subscribers.begin(), subscribers.end(),
                [paramId](const Subscriber& s) { return s.paramId == paramId; });
            const size_t removed = static_cast<size_t>(std::distance(newEnd, subscribers.end()));
            subscribers.erase(newEnd, subscribers.end());
            return removed;
        }

        void UnsubscribeAll() { subscribers.clear(); }

        bool HasSubscribers() const { return !subscribers.empty(); }

        bool HasSubscribers(uint64_t paramId) const {
            return std::any_of(subscribers.begin(), subscribers.end(),
                [paramId](const Subscriber& s) {
                    return s.paramId == paramId || s.paramId == kAnyParamId;
                });
        }

        size_t GetSubscriberCount() const { return subscribers.size(); }

        ExecutorTypeRHI GetExecutorType() const { return type; }
        unsigned int GetLayer() const override { return layer; }
        void SetLayer(unsigned int newLayer) override { layer = newLayer; }

        const std::vector<ParamTokenT>& GetParams() const { return params; }

        uint64_t AddParam(Params param, int paramLayer = 0, bool removeAfterExecute = false, bool enabled = true) {
            const uint64_t id = nextParamId.fetch_add(1);
            params.emplace_back(id, paramLayer, std::move(param), removeAfterExecute, enabled);
            return id;
        }

        template<typename... Args>
        uint64_t EmplaceParam(int paramLayer, bool removeAfterExecute, bool enabled, Args&&... args) {
            const uint64_t id = nextParamId.fetch_add(1);
            params.emplace_back(id, paramLayer, Params(std::forward<Args>(args)...), removeAfterExecute, enabled);
            return id;
        }

        template<typename ParamT>
        uint64_t AddParamAndMark(ParamT&& p, int paramLayer = 0, bool removeAfterExecute = false) {
            const uint64_t id = AddParam(Params{std::forward<ParamT>(p)}, paramLayer, removeAfterExecute);
            MarkProcess(true);
            return id;
        }

        void RemoveParam(size_t index) {
            if (index >= params.size()) return;
            params.erase(params.begin() + static_cast<ptrdiff_t>(index));
        }

        bool RemoveParamById(uint64_t id) {
            auto it = std::find_if(params.begin(), params.end(),
                [id](const ParamTokenT& p) { return p.id == id; });
            if (it == params.end()) return false;
            params.erase(it);
            return true;
        }

        void ClearParams() {
            params.clear();
            auto newEnd = std::remove_if(subscribers.begin(), subscribers.end(),
                [](const Subscriber& s) { return s.paramId != kAnyParamId; });
            subscribers.erase(newEnd, subscribers.end());
        }

        void ReserveParams(size_t count) { params.reserve(count); }

        size_t GetParamCount() const { return params.size(); }
        bool  HasParams() const { return !params.empty(); }

        ParamTokenT& GetParamToken(size_t index) {
            if (index >= params.size()) throw std::out_of_range("Param index out of range");
            return params[index];
        }
        const ParamTokenT& GetParamToken(size_t index) const {
            if (index >= params.size()) throw std::out_of_range("Param index out of range");
            return params[index];
        }

        Params& GetParam(size_t index) {
            return GetParamToken(index).params;
        }
        const Params& GetParam(size_t index) const {
            return GetParamToken(index).params;
        }

        void SetParamLayer(size_t index, int newLayer) {
            GetParamToken(index).layer = newLayer;
        }
        void SetParamRemoveAfterExecute(size_t index, bool value) {
            GetParamToken(index).removeAfterExecute = value;
        }
        void SetParamEnabled(size_t index, bool value) {
            GetParamToken(index).enabled = value;
        }
        void SetParamFlags(size_t index, int newLayer, bool removeAfterExecute, bool enabled) {
            auto& p = GetParamToken(index);
            p.layer = newLayer;
            p.removeAfterExecute = removeAfterExecute;
            p.enabled = enabled;
        }

        template<typename T>
        std::optional<T> GetParamByType() const {
            for (const auto& p : params) {
                if (std::holds_alternative<T>(p.params)) {
                    return std::get<T>(p.params);
                }
            }
            return std::nullopt;
        }

        template<typename T>
        std::vector<size_t> FindParamIndicesByType() const {
            std::vector<size_t> result;
            for (size_t i = 0; i < params.size(); i++) {
                if (std::holds_alternative<T>(params[i].params)) result.push_back(i);
            }
            return result;
        }

        std::vector<size_t> GetParamIndicesByLayer(int layerFilter) const {
            std::vector<size_t> result;
            for (size_t i = 0; i < params.size(); i++) {
                if (params[i].layer == layerFilter) result.push_back(i);
            }
            return result;
        }

        std::vector<size_t> GetEnabledParamIndicesByLayer(int layerFilter) const {
            std::vector<size_t> result;
            for (size_t i = 0; i < params.size(); i++) {
                if (params[i].layer == layerFilter && params[i].enabled)
                    result.push_back(i);
            }
            return result;
        }

        std::optional<std::pair<int, int>> GetLayerRange() const {
            if (params.empty()) return std::nullopt;
            int lo = params.front().layer;
            int hi = params.front().layer;
            for (const auto& p : params) {
                lo = std::min(lo, p.layer);
                hi = std::max(hi, p.layer);
            }
            return std::make_pair(lo, hi);
        }

        void MarkProcess(bool newValue) override { markProcess = newValue; }
        bool IsMarkedToProcess() const override { return markProcess; }

        void Reset() {
            params.clear();
            UnsubscribeAll();
            layer = 0;
            markProcess = false;
        }

        void ResetHard() {
            Reset();
            nextParamId.store(1);
            nextSubscriptionId.store(1);
        }
    };
}