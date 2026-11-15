#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"

namespace RHI::Executors::SwapChainExecutor {
    struct SwapChainCreateResult {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t bufferCount = 0;
        PresentMode presentMode = PresentMode::FIFO;
        ColorSpace colorSpace = ColorSpace::SRGB_NONLINEAR;
        bool success = true;
        std::string errorMessage;
    };

    struct SwapChainResizeResult {
        uint32_t oldWidth = 0;
        uint32_t oldHeight = 0;
        uint32_t newWidth = 0;
        uint32_t newHeight = 0;
        uint32_t bufferCount = 0;
        bool success = true;
        std::string errorMessage;
    };

    struct SwapChainPresentResult {
        PresentMode presentMode = PresentMode::FIFO;
        uint64_t frameNumber = 0;
        bool success = true;
        std::string errorMessage;
    };

    struct SwapChainDestroyResult {
        bool success = true;
        std::string errorMessage;
    };

    struct SwapChainExecuteResult : public BaseProcessResult {
        enum class OperationType {
            CREATE,
            RESIZE,
            PRESENT,
            DESTROY,
            UNKNOWN
        };

        OperationType operationType{OperationType::UNKNOWN};

        std::variant<
            SwapChainCreateResult,
            SwapChainResizeResult,
            SwapChainPresentResult,
            SwapChainDestroyResult
        > data;

        SwapChainExecuteResult() = default;

        SwapChainExecuteResult(const SwapChainCreateResult& r)
            : operationType(OperationType::CREATE), data(r) {}

        SwapChainExecuteResult(const SwapChainResizeResult& r)
            : operationType(OperationType::RESIZE), data(r) {}

        SwapChainExecuteResult(const SwapChainPresentResult& r)
            : operationType(OperationType::PRESENT), data(r) {}

        SwapChainExecuteResult(const SwapChainDestroyResult& r)
            : operationType(OperationType::DESTROY), data(r) {}
    };
}