#pragma once

#define RHI_ONCE(exec, id, T, var, body) \
    (exec)->OnceTyped<T>((id), [&](const T& var) { body })

#define RHI_ON(exec, id, T, var, body) \
    (exec)->On<T>((id), [&](const T& var) { body })

#define RHI_ONCE_ALL(exec, results, body) \
    (exec)->OnceAll([&](const std::vector<std::decay_t<decltype(*results.begin())>>& results) { body })

#define RHI_ON_ALL(exec, results, body) \
    (exec)->OnAll([&](const std::vector<std::decay_t<decltype(*results.begin())>>& results) { body })

#define RHI_ONCE_ON_LAYER(exec, id, layer, T, var, body) \
    (exec)->SubscribeOnce((id), [&](const std::vector<std::decay_t<decltype(T{})>>& __rhi_rs) { \
        for (const auto& __rhi_r : __rhi_rs) { \
            if (__rhi_r.layer != (layer)) continue; \
            if (auto* __rhi_p = std::get_if<T>(&__rhi_r.data)) { \
                const T& var = *__rhi_p; body; \
            } \
        } \
    })

#define RHI_ON_LAYER(exec, id, layer, T, var, body) \
    (exec)->SubscribeOnLayer((id), (layer), [&](const std::vector<std::decay_t<decltype(T{})>>& __rhi_rs) { \
        for (const auto& __rhi_r : __rhi_rs) { \
            if (auto* __rhi_p = std::get_if<T>(&__rhi_r.data)) { \
                const T& var = *__rhi_p; body; \
            } \
        } \
    })

#define RHI_ONCE_RESULT(exec, id, T, var) \
    (exec)->SubscribeOnce((id), [&](const std::vector<std::decay_t<decltype(T{})>>& __rhi_rs) { \
        for (const auto& __rhi_r : __rhi_rs) { \
            if (auto* __rhi_p = std::get_if<T>(&__rhi_r.data)) { \
                const T& var = *__rhi_p;

#define RHI_ON_RESULT(exec, id, T, var) \
    (exec)->Subscribe((id), [&](const std::vector<std::decay_t<decltype(T{})>>& __rhi_rs) { \
        for (const auto& __rhi_r : __rhi_rs) { \
            if (auto* __rhi_p = std::get_if<T>(&__rhi_r.data)) { \
                const T& var = *__rhi_p;

#define RHI_END_ON_RESULT \
            } \
        } \
    })

#define RHI_ONCE_ALL_RESULTS(exec, results) \
    (exec)->SubscribeAllOnce([&](const std::vector<std::decay_t<decltype(*results.begin())>>& results) {

#define RHI_ON_ALL_RESULTS(exec, results) \
    (exec)->SubscribeAll([&](const std::vector<std::decay_t<decltype(*results.begin())>>& results) {

#define RHI_END_ON_RESULTS \
    })

#define RHI_SCOPED_ONCE(exec, id, T, var, body) \
    auto __rhi_scope_##__LINE__ = (exec)->ScopedOnce((id), [&](const T& var) { body })

#define RHI_SCOPED_ON(exec, id, T, var, body) \
    auto __rhi_scope_##__LINE__ = (exec)->ScopedOn((id), [&](const T& var) { body })

#define RHI_SCOPED_ON_ALL(exec, body) \
    auto __rhi_scope_##__LINE__ = (exec)->ScopedOnAll([&](const auto& __rhi_rs) { body })