#pragma once

#include <concepts>
#include <vector>
#include <algorithm>
#include <optional>
#include <functional>
#include <variant>
#include <type_traits>
#include <atomic>
#include <stdexcept>
#include <string>
#include <limits>
#include <utility>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"
#include "RHI/Base/Executors/IExecutorRHI.h"

namespace RHI::Executors {
    template<typename T>
    concept HasProcessCurrentParam = requires(T* t, RHI::Base::BaseDevice& device,
                                            const typename T::ParamsType& p) {
        { t->ProcessCurrentParam(device, p) } -> std::same_as<typename T::ResultType>;
    };

    template<typename T>
    struct is_variant : std::false_type {};
    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};
    template<typename T>
    inline constexpr bool is_variant_v = is_variant<T>::value;

    inline constexpr uint64_t AnyParamId = std::numeric_limits<uint64_t>::max();

    struct BaseProcessResult {
        uint64_t paramId = AnyParamId;
        int layer = -1;
        bool success{true};
        std::string errorMessage = "";

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

        void ProcessFailed(const std::string& newErrorMessage) {
            success = false;
            errorMessage = newErrorMessage;
        }
    };

    struct SubscriptionToken {
        uint64_t id{0};
        uint64_t paramId{AnyParamId};

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

    class ScopedSubscription {
        IExecutorRHI* owner = nullptr;
        SubscriptionToken token{};
    public:
        ScopedSubscription() = default;
        ScopedSubscription(IExecutorRHI* o, SubscriptionToken t) : owner(o), token(t) {}
        ScopedSubscription(ScopedSubscription&& o) noexcept
            : owner(o.owner), token(o.token) { o.owner = nullptr; }
        ScopedSubscription& operator=(ScopedSubscription&& o) noexcept {
            if (this != &o) { Release(); owner = o.owner; token = o.token; o.owner = nullptr; }
            return *this;
        }
        ScopedSubscription(const ScopedSubscription&) = delete;
        ScopedSubscription& operator=(const ScopedSubscription&) = delete;
        ~ScopedSubscription() { Release(); }

        void Release() {
            if (owner && token.IsValid()) owner->Unsubscribe(token);
            owner = nullptr;
            token = SubscriptionToken{};
        }

        void Reset() { Release(); }

        void Swap(ScopedSubscription& o) noexcept {
            std::swap(owner, o.owner);
            std::swap(token, o.token);
        }

        const SubscriptionToken& Get() const { return token; }
        bool IsValid() const noexcept { return owner != nullptr && token.IsValid(); }
    };

    template<typename Executor>
    class ParamBuilder {
        static_assert(std::is_base_of_v<IExecutorRHI, Executor>,
                    "Executor must derive from IExecutorRHI");

    private:
        using ParamsT = typename Executor::ParamsType;

        Executor* owner = nullptr;
        ParamsT value;
        int layer = 0;
        bool removeAfterExecute = false;
        bool enabled = true;
        bool markProcess = false;

    public:
        ParamBuilder(Executor* o, ParamsT v)
            : owner(o), value(std::move(v)) {}

        ParamBuilder& OnLayer(int l) noexcept { layer = l; return *this; }
        ParamBuilder& RemoveAfterExecute(bool v = true) noexcept { removeAfterExecute = v; return *this; }
        ParamBuilder& Enabled(bool v = true) noexcept { enabled = v; return *this; }
        ParamBuilder& Disabled(bool v = true) noexcept { enabled = !v; return *this; }
        ParamBuilder& MarkProcess(bool v = true) noexcept { markProcess = v; return *this; }

        uint64_t Commit() {
            if (!owner) return 0;

            const uint64_t id = owner->AddParam(std::move(value), layer, removeAfterExecute, enabled);
            if (markProcess) owner->MarkProcess(true);
            return id;
        }
    };

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

        bool isNotifying{false};

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

            std::vector<Subscriber*> snapshot;
            snapshot.reserve(subscribers.size());
            for (auto& s : subscribers) snapshot.push_back(&s);

            std::vector<uint64_t> toRemoveIds;
            toRemoveIds.reserve(snapshot.size());

            isNotifying = true;

            for (auto* sub : snapshot) {
                auto it = std::find_if(subscribers.begin(), subscribers.end(),
                    [&](const Subscriber& s) { return s.id == sub->id; });
                if (it == subscribers.end()) continue;
                sub = &*it;

                if (!sub->enabled) continue;

                if (sub->paramId == AnyParamId && sub->layerFilter == -1) {
                    sub->callback(results);
                }
                else {
                    std::vector<ProcessResult> filtered;
                    filtered.reserve(results.size());
                    for (const auto& r : results) {
                        if (sub->paramId != AnyParamId && r.paramId != sub->paramId) continue;
                        if (sub->layerFilter != -1 && r.layer != sub->layerFilter) continue;
                        filtered.push_back(r);
                    }
                    if (filtered.empty()) continue;
                    sub->callback(filtered);
                }

                if (sub->isOnce) toRemoveIds.push_back(sub->id);
            }

            isNotifying = false;

            if (!toRemoveIds.empty()) {
                subscribers.erase(
                    std::remove_if(subscribers.begin(), subscribers.end(),
                        [&](const Subscriber& s) {
                            return std::find(toRemoveIds.begin(), toRemoveIds.end(), s.id)
                                != toRemoveIds.end();
                        }),
                    subscribers.end());
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

            for (auto& token : params) {
                if (!ShouldProcessParam(token)) continue;

                ProcessResult r = ProcessSingleParam(device, token.params);
                r.paramId = token.id;
                r.layer = token.layer;
                results.push_back(std::move(r));
            }

            PruneAutoRemoveParams();
            NotifySubscribers(results);
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
            return SubscribeInternal(AnyParamId, -1, std::move(cb), false);
        }
        SubscriptionToken SubscribeAllOnce(CallbackType cb) {
            return SubscribeInternal(AnyParamId, -1, std::move(cb), true);
        }

        SubscriptionToken SubscribeOnLayer(uint64_t paramId, int layer, CallbackType cb) {
            return SubscribeInternal(paramId, layer, std::move(cb), false);
        }
        SubscriptionToken SubscribeAllOnLayer(int layer, CallbackType cb) {
            return SubscribeInternal(AnyParamId, layer, std::move(cb), false);
        }

        bool SetSubscriptionEnabled(const SubscriptionToken& token, bool enabled) {
            auto it = std::find_if(subscribers.begin(), subscribers.end(),
                [&](const Subscriber& s) { return s.id == token.id && s.paramId == token.paramId; });
            if (it == subscribers.end()) return false;
            it->enabled = enabled;
            return true;
        }

        bool Unsubscribe(const SubscriptionToken& token) override {
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
                    return s.paramId == paramId || s.paramId == AnyParamId;
                });
        }

        size_t GetSubscriberCount() const { return subscribers.size(); }

        ExecutorTypeRHI GetExecutorType() const override { return type; }
        unsigned int GetLayer() const override { return layer; }
        void SetLayer(unsigned int newLayer) override { layer = newLayer; }

        const std::vector<ParamTokenT>& GetParams() const { return params; }

        uint64_t AddParam(Params&& param, int paramLayer = 0, bool removeAfterExecute = false, bool enabled = true) {
            const uint64_t id = nextParamId.fetch_add(1);
            params.emplace_back(id, paramLayer, std::move(param), removeAfterExecute, enabled);
            return id;
        }

        uint64_t AddParam(const Params& param, int paramLayer = 0, bool removeAfterExecute = false, bool enabled = true) {
            Params copy = param;
            return AddParam(std::move(copy), paramLayer, removeAfterExecute, enabled);
        }

        template<typename ParamT>
        uint64_t AddParamDirect(ParamT&& p, int paramLayer = 0, bool removeAfterExecute = false) {
            const uint64_t id = nextParamId.fetch_add(1);
            params.emplace_back(id, paramLayer, Params{ std::forward<ParamT>(p) }, removeAfterExecute, true);
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

        template<typename ParamT>
        ParamBuilder<ExecutorImplRHI> Add(ParamT&& p) {
            return ParamBuilder<ExecutorImplRHI>(impl(), Params{ std::forward<ParamT>(p) });
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
                [](const Subscriber& s) { return s.paramId != AnyParamId; });
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

        void MarkAndProcess(RHI::Base::BaseDevice& device) {
            MarkProcess(true);
            ProcessParams(device);
        }

        void operator()(RHI::Base::BaseDevice& device) { ProcessParams(device); }

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

        template<typename F>
        SubscriptionToken OnceTyped(uint64_t paramId, F&& f) {
            return SubscribeOnce(paramId,
                [f = std::forward<F>(f)](const std::vector<ProcessResult>& results) mutable {
                    for (const auto& r : results)
                        std::visit([&](auto& v) {
                            using T = std::decay_t<decltype(v)>;
                            if constexpr (std::is_invocable_v<F, T&>) { f(v); }
                        }, r.data);
                });
        }

        SubscriptionToken On(uint64_t paramId, CallbackType cb) { return Subscribe(paramId, std::move(cb)); }
        SubscriptionToken Once(uint64_t paramId, CallbackType cb) { return SubscribeOnce(paramId, std::move(cb)); }
        SubscriptionToken OnAll(CallbackType cb) { return SubscribeAll(std::move(cb)); }
        SubscriptionToken OnceAll(CallbackType cb) { return SubscribeAllOnce(std::move(cb)); }
        SubscriptionToken OnLayer(uint64_t paramId, int l, CallbackType cb) { return SubscribeOnLayer(paramId, l, std::move(cb)); }
        SubscriptionToken OnAllLayer(int l, CallbackType cb) { return SubscribeAllOnLayer(l, std::move(cb)); }

        template<typename F> ScopedSubscription ScopedOn(uint64_t id, F&& f)
            { return { this, On(id, std::forward<F>(f)) }; }
        template<typename F> ScopedSubscription ScopedOnAll(F&& f)
            { return { this, OnAll(std::forward<F>(f)) }; }
        template<typename F> ScopedSubscription ScopedOnce(uint64_t id, F&& f)
            { return { this, Once(id, std::forward<F>(f)) }; }

        template<typename F> void ForEachParam(F&& f) {
            for (auto& t : params) f(t);
        }

        template<typename F> void ForEachParamOfType(F&& f) {
            for (auto& t : params) {
                std::visit([&](auto& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_invocable_v<F, T&>) f(v);
                }, t.params);
            }
        }

        template<typename T> T* GetParamPtrByType() {
            for (auto& p : params)
                if (auto* v = std::get_if<T>(&p.params)) return v;
            return nullptr;
        }

        template<typename T> const T* GetParamPtrByType() const {
            for (const auto& p : params)
                if (auto* v = std::get_if<T>(&p.params)) return v;
            return nullptr;
        }

        template<typename T> std::vector<T> GetAllParamsByType() const {
            std::vector<T> out;
            for (const auto& p : params)
                if (auto* v = std::get_if<T>(&p.params)) out.push_back(*v);
            return out;
        }

        template<typename T> std::vector<const T*> GetAllParamPtrsByType() const {
            std::vector<const T*> out;
            for (const auto& p : params)
                if (auto* v = std::get_if<T>(&p.params)) out.push_back(v);
            return out;
        }

        template<typename T, typename F>
        SubscriptionToken OnceTyped(uint64_t paramId, F&& f) {
            return SubscribeOnce(paramId,
                [f = std::forward<F>(f)](const std::vector<ProcessResult>& results) mutable {
                    for (const auto& r : results)
                        if (auto* v = std::get_if<T>(&r.data)) { f(*v); return; }
                });
        }

        ParamBuilder<ExecutorImplRHI> Add(Params p) {
            return ParamBuilder<ExecutorImplRHI>(impl(), std::move(p));
        }

        template<typename ParamT>
        ParamBuilder<ExecutorImplRHI> AddT(ParamT&& p) {
            return ParamBuilder<ExecutorImplRHI>(impl(), Params{ std::forward<ParamT>(p) });
        }
    };
}