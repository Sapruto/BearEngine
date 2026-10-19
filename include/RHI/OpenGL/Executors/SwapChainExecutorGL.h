#pragma once

#include "RHI/Base/Executors/SwapChainExecutor.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Contracts/SwapChainExecutorResult.h"
#include "RHI/Base/Core/BaseDevice.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"

namespace RHI::Executors::SwapChainExecutor {
    class SwapChainExecutorGL final : public SwapChainExecutor<SwapChainExecutorGL> {
    private:
        GLFWwindow* window = nullptr;

        uint32_t currentWidth = 0;
        uint32_t currentHeight = 0;
        uint32_t currentBufferCount = 2;

        PresentMode currentPresentMode = PresentMode::FIFO;
        ColorSpace currentColorSpace = ColorSpace::SRGB_NONLINEAR;

        uint64_t frameCounter = 0;

    public:
        SwapChainExecuteResult ProcessCreateSwapChainImpl(Base::BaseDevice& device, const SwapChainCreateParams& params);
        SwapChainExecuteResult ProcessResizeSwapChainImpl(Base::BaseDevice& device, const SwapChainResizeParams& params);
        SwapChainExecuteResult ProcessPresentSwapChainImpl(Base::BaseDevice& device, const SwapChainPresentParams& params);
        SwapChainExecuteResult ProcessDestroySwapChainImpl(Base::BaseDevice& device, const SwapChainDestroyParams& params);

        bool IsValid() override;
    };
}