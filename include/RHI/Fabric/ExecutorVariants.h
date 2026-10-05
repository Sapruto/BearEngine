#pragma once

#include <variant>
#include "RHI/Fabric/BackendTraits.h"

namespace RHI::FabricRHI {
    template<typename Backend=CurrentBackend>
    using BufferExecutorVariant = std::variant<
        std::monostate,
        typename Backend::BufferExecutor*
    >;

    template<typename Backend=CurrentBackend>
    using DrawExecutorVariant = std::variant<
        std::monostate,
        typename Backend::DrawExecutor*
    >;

    template<typename Backend=CurrentBackend>
    using SwapChainExecutorVariant = std::variant<
        std::monostate,
        typename Backend::SwapChainExecutor*
    >;

    template<typename Backend=CurrentBackend>
    using AnyExecutorVariant = std::variant<
        std::monostate,
        typename Backend::BufferExecutor*,
        typename Backend::DrawExecutor*,
        typename Backend::SwapChainExecutor*
    >;
}