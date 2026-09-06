#pragma once

#include <variant>

namespace SwapChainExecute {
    enum class IntervalType {
        FIFO,
        IMMEDIATE,
        FIFO_RELAXED,
        MAILBOX
    };

    enum class SwapChainAction {
        Create,
        Resize,
        Present,
        Destroy
    };

    struct SwapChainCreateParams {
        void* windowHandle;

        uint32_t width;
        uint32_t height;
        uint32_t swapChainSize;
        IntervalType intervalType;
        
        float colorSpace;
    };

    struct SwapChainResizeParams {
        uint32_t newWidth;
        uint32_t newHeight;
        bool preserveContent;
    };

    struct SwapChainPresentParams {
        uint32_t syncInterval;
        IntervalType intervalType;
    };

    struct SwapChainDestroyParams {
        bool forceImmediate;
    };

    using SwapChainParams = std::variant<
        SwapChainCreateParams,
        SwapChainResizeParams,
        SwapChainPresentParams,
        SwapChainDestroyParams
    >;
}