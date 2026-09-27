#pragma once

#include <variant>

namespace FabricRHI {
    template<typename Backend>
    using BufferExecuterVariant = std::variant<
        std::monostate,
        typename Backend::BufferExecuter*
    >;

    template<typename Backend>
    using DrawExecuterVariant = std::variant<
        std::monostate,
        typename Backend::DrawExecuter*
    >;

    template<typename Backend>
    using SwapChainExecuterVariant = std::variant<
        std::monostate,
        typename Backend::SwapChainExecuter*
    >;

    template<typename Backend>
    using AnyExecuterVariant = std::variant<
        std::monostate,
        typename Backend::BufferExecuter*,
        typename Backend::DrawExecuter*,
        typename Backend::SwapChainExecuter*
    >;
}