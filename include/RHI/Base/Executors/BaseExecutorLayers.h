#pragma once

#include <cstdint>

namespace RHI::Executors {
    enum class BaseExecutorLayers : unsigned int {
        Buffer = 1,
        Draw = 2,
        SwapChain = 3
    };
}