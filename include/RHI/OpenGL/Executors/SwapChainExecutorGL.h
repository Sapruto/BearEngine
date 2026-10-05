#pragma once

#include "RHI/Base/Executors/SwapChainExecutor.h"
#include "RHI/Base/Models/SwapChainExecutorParams.h"
#include "RHI/Base/Models/SwapChainExecutorResult.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <chrono>

namespace RHI::Executors::SwapChainExecute {
    class SwapChainExecutorGL final : public SwapChainExecutor<SwapChainExecutorGL> {
    private:
        bool CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode);
        GLenum GetGLInterval(IntervalType interval);
        
        GLFWwindow* window{nullptr};
        uint32_t currentWidth{0};
        uint32_t currentHeight{0};
        uint32_t swapChainSize{2};
        IntervalType currentInterval{IntervalType::FIFO};
        uint64_t frameCounter{0};
        uint64_t presentCounter{0};
        double lastPresentTime{0.0};

    public:
        SwapChainExecuteResult ProcessSwapChainCreateImpl(RHI::Base::BaseDevice& device, const SwapChainCreateParams& params);
        SwapChainExecuteResult ProcessSwapChainResizeImpl(RHI::Base::BaseDevice& device, const SwapChainResizeParams& params);
        SwapChainExecuteResult ProcessSwapChainPresentImpl(RHI::Base::BaseDevice& device, const SwapChainPresentParams& params);
        SwapChainExecuteResult ProcessSwapChainDestroyImpl(RHI::Base::BaseDevice& device, const SwapChainDestroyParams& params);
        
        bool IsValid() override;
    };
}