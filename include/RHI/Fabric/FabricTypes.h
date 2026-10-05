#pragma once

#include <cstdint>

namespace RHI::FabricRHI {
    enum class BackendType : uint8_t {
        OpenGL,
        Vulkan,
        USE_BASE,
        COUNT,
    };
}