#pragma once

#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"

namespace SwapChainExecute {
    enum class IntervalType {
        FIFO,
        IMMEDIATE,
        FIFO_RELAXED,
        MAILBOX
    };

    enum class ParamType {
        CREATE,
        RESIZE,
        PRESENT,
        DESTROY
    };

    struct BaseSwapChainParams {
    private:
        ParamType type;

    protected:
        void SetType(ParamType newType) { type = newType; }

    public:
        BaseSwapChainParams() : type(ParamType::CREATE) {}
        virtual ~BaseSwapChainParams() = default;
        
        ParamType GetType() const { return type; }
    };

    struct SwapChainCreateParams : public BaseSwapChainParams {
        void* windowHandle;
        uint32_t width;
        uint32_t height;
        uint32_t swapChainSize;
        IntervalType intervalType;
        float colorSpace;

        SwapChainCreateParams() {
            SetType(ParamType::CREATE);
        }

        SwapChainCreateParams(void* windowHandle, uint32_t width, uint32_t height,
                              uint32_t swapChainSize, IntervalType intervalType, float colorSpace)
            : windowHandle(windowHandle), width(width), height(height),
              swapChainSize(swapChainSize), intervalType(intervalType), colorSpace(colorSpace) {
            SetType(ParamType::CREATE);
        }
    };

    struct SwapChainResizeParams : public BaseSwapChainParams {
        uint32_t newWidth;
        uint32_t newHeight;
        bool preserveContent;

        SwapChainResizeParams() {
            SetType(ParamType::RESIZE);
        }

        SwapChainResizeParams(uint32_t newWidth, uint32_t newHeight, bool preserveContent)
            : newWidth(newWidth), newHeight(newHeight), preserveContent(preserveContent) {
            SetType(ParamType::RESIZE);
        }
    };

    struct SwapChainPresentParams : public BaseSwapChainParams {
        uint32_t syncInterval;
        IntervalType intervalType;

        SwapChainPresentParams() {
            SetType(ParamType::PRESENT);
        }

        SwapChainPresentParams(uint32_t syncInterval, IntervalType intervalType)
            : syncInterval(syncInterval), intervalType(intervalType) {
            SetType(ParamType::PRESENT);
        }
    };

    struct SwapChainDestroyParams : public BaseSwapChainParams {
        bool forceImmediate;

        SwapChainDestroyParams() {
            SetType(ParamType::DESTROY);
        }

        SwapChainDestroyParams(bool forceImmediate)
            : forceImmediate(forceImmediate) {
            SetType(ParamType::DESTROY);
        }
    };

    using SwapChainParams = std::variant<
        SwapChainCreateParams,
        SwapChainResizeParams,
        SwapChainPresentParams,
        SwapChainDestroyParams
    >;
}