#pragma once

#include <cstdint>
#include <variant>

#include "RHI/Base/Handles.h"

namespace RHI::Executors::SwapChainExecutor {
    enum class PresentMode {
        FIFO,
        IMMEDIATE,
        FIFO_RELAXED,
        MAILBOX
    };

    enum class ColorSpace {
        SRGB_NONLINEAR,
        LINEAR,
        HDR10_ST2084,
        DISPLAY_P3
    };

    struct SwapChainCreateParams {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t bufferCount = 2;
        PresentMode presentMode = PresentMode::FIFO;
        ColorSpace colorSpace = ColorSpace::SRGB_NONLINEAR;
    };

    struct SwapChainResizeParams {
        uint32_t newWidth = 0;
        uint32_t newHeight = 0;
        bool preserveContent = false;
    };

    struct SwapChainPresentParams {
        PresentMode presentMode = PresentMode::FIFO;
        RHI::Handles::RenderTargetHandle renderTarget{};
    };

    struct SwapChainDestroyParams {
        bool forceImmediate = false;
    };

    using SwapChainParams = std::variant<
        SwapChainCreateParams,
        SwapChainResizeParams,
        SwapChainPresentParams,
        SwapChainDestroyParams
    >;
}