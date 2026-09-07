#pragma once

#include <vector>
#include <string>
#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"

namespace SwapChainExecute {
    struct SwapChainCreateResult {
        void* windowHandle;
        uint32_t width;
        uint32_t height;
        uint32_t swapChainSize;
        uint32_t actualSwapChainSize;
        uint32_t bufferCount;
        IntervalType intervalType;
        bool isCreated;
        bool isInitialized;
        std::string errorMessage;
    };

    struct SwapChainResizeResult {
        uint32_t oldWidth;
        uint32_t oldHeight;
        uint32_t newWidth;
        uint32_t newHeight;
        bool isResized;
        uint32_t backBufferCount;
        std::string errorMessage;
    };

    struct SwapChainPresentResult {
        uint32_t syncInterval;
        IntervalType intervalType;
        bool isPresented;
        uint64_t frameNumber;
        double presentationTimeMs;
        double gpuTimeMs;
        uint64_t presentCounter;
        std::string errorMessage;
    };

    struct SwapChainDestroyResult {
        bool forceImmediate;
        bool isDestroyed;
        bool resourcesFreed;
        size_t freedMemoryBytes;
        std::string errorMessage;
    };

    struct SwapChainPerformance {
        double fps{0.0};
        double frameTimeMs{0.0};
        double gpuFrameTimeMs{0.0};
        double cpuFrameTimeMs{0.0};
        uint64_t totalFrames{0};
        uint32_t droppedFrames{0};
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
        
        SwapChainPerformance performance;
        
        std::variant<SwapChainCreateResult, SwapChainResizeResult, SwapChainPresentResult, SwapChainDestroyResult> data;

        SwapChainExecuteResult(const SwapChainCreateResult& r) : operationType(OperationType::CREATE), data(r) {}
        SwapChainExecuteResult(const SwapChainResizeResult& r) : operationType(OperationType::RESIZE), data(r) {}
        SwapChainExecuteResult(const SwapChainPresentResult& r) : operationType(OperationType::PRESENT), data(r) {}
        SwapChainExecuteResult(const SwapChainDestroyResult& r) : operationType(OperationType::DESTROY), data(r) {}
        
        SwapChainExecuteResult() = default;
    };
}