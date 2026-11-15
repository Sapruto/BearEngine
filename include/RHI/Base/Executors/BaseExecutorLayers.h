#pragma once

#include <cstdint>

namespace RHI::Executors {
    enum class BaseExecutorLayers : unsigned int {
        Resources = 1,
        Pipeline = 2,
        Draw = 3,
        SwapChain = 4
    };
}