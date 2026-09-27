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

        SwapChainCreateResult() = default;
        SwapChainCreateResult(void* windowHandle, uint32_t width, uint32_t height,
                              uint32_t swapChainSize, uint32_t actualSwapChainSize,
                              uint32_t bufferCount, IntervalType intervalType,
                              bool isCreated, bool isInitialized,
                              const std::string& errorMessage = "")
            : windowHandle(windowHandle), width(width), height(height),
              swapChainSize(swapChainSize), actualSwapChainSize(actualSwapChainSize),
              bufferCount(bufferCount), intervalType(intervalType),
              isCreated(isCreated), isInitialized(isInitialized),
              errorMessage(errorMessage) {}
    };

    struct SwapChainResizeResult {
        uint32_t oldWidth;
        uint32_t oldHeight;
        uint32_t newWidth;
        uint32_t newHeight;
        bool isResized;
        uint32_t backBufferCount;
        std::string errorMessage;

        SwapChainResizeResult() = default;
        SwapChainResizeResult(uint32_t oldWidth, uint32_t oldHeight, uint32_t newWidth,
                              uint32_t newHeight, bool isResized, uint32_t backBufferCount,
                              const std::string& errorMessage = "")
            : oldWidth(oldWidth), oldHeight(oldHeight), newWidth(newWidth),
              newHeight(newHeight), isResized(isResized), backBufferCount(backBufferCount),
              errorMessage(errorMessage) {}
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

        SwapChainPresentResult() = default;
        SwapChainPresentResult(uint32_t syncInterval, IntervalType intervalType, bool isPresented,
                               uint64_t frameNumber, double presentationTimeMs, double gpuTimeMs,
                               uint64_t presentCounter, const std::string& errorMessage = "")
            : syncInterval(syncInterval), intervalType(intervalType), isPresented(isPresented),
              frameNumber(frameNumber), presentationTimeMs(presentationTimeMs),
              gpuTimeMs(gpuTimeMs), presentCounter(presentCounter),
              errorMessage(errorMessage) {}
    };

    struct SwapChainDestroyResult {
        bool forceImmediate;
        bool isDestroyed;
        bool resourcesFreed;
        size_t freedMemoryBytes;
        std::string errorMessage;

        SwapChainDestroyResult() = default;
        SwapChainDestroyResult(bool forceImmediate, bool isDestroyed, bool resourcesFreed,
                               size_t freedMemoryBytes, const std::string& errorMessage = "")
            : forceImmediate(forceImmediate), isDestroyed(isDestroyed),
              resourcesFreed(resourcesFreed), freedMemoryBytes(freedMemoryBytes),
              errorMessage(errorMessage) {}
    };

    struct SwapChainPerformance {
        double fps{0.0};
        double frameTimeMs{0.0};
        double gpuFrameTimeMs{0.0};
        double cpuFrameTimeMs{0.0};
        uint64_t totalFrames{0};
        uint32_t droppedFrames{0};

        SwapChainPerformance() = default;
        SwapChainPerformance(double fps, double frameTimeMs, double gpuFrameTimeMs,
                             double cpuFrameTimeMs, uint64_t totalFrames, uint32_t droppedFrames)
            : fps(fps), frameTimeMs(frameTimeMs), gpuFrameTimeMs(gpuFrameTimeMs),
              cpuFrameTimeMs(cpuFrameTimeMs), totalFrames(totalFrames),
              droppedFrames(droppedFrames) {}
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

        SwapChainExecuteResult() = default;

        SwapChainExecuteResult(const SwapChainCreateResult& r) : operationType(OperationType::CREATE), data(r) {}
        SwapChainExecuteResult(const SwapChainResizeResult& r) : operationType(OperationType::RESIZE), data(r) {}
        SwapChainExecuteResult(const SwapChainPresentResult& r) : operationType(OperationType::PRESENT), data(r) {}
        SwapChainExecuteResult(const SwapChainDestroyResult& r) : operationType(OperationType::DESTROY), data(r) {}
    };
}