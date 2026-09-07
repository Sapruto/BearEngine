#pragma once

#include "RHI/Base/Executers/SwapChainExecuter.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <chrono>

namespace SwapChainExecute {
    class SwapChainGL : public SwapChainExecuter<SwapChainGL> {
    private:
        GLFWwindow* GetWindow(void* windowHandle);
        
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

    protected:
        SwapChainExecuteResult ProcessSwapChainCreateImpl(BaseDevice& device, const SwapChainCreateParams& params);
        SwapChainExecuteResult ProcessSwapChainResizeImpl(BaseDevice& device, const SwapChainResizeParams& params);
        SwapChainExecuteResult ProcessSwapChainPresentImpl(BaseDevice& device, const SwapChainPresentParams& params);
        SwapChainExecuteResult ProcessSwapChainDestroyImpl(BaseDevice& device, const SwapChainDestroyParams& params);
        
        bool IsValid();
    };
}