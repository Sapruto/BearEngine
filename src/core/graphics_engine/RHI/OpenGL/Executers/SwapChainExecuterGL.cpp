#include "RHI/OpenGL/Executers/SwapChainGL.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseWindow.h"

#include <iostream>

namespace SwapChainExecute {
    GLFWwindow* SwapChainGL::GetWindow(void* windowHandle) {
        return static_cast<GLFWwindow*>(windowHandle);
    }

    bool SwapChainGL::CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            errorMsg = std::string("OpenGL Error ") + op + ": " + std::to_string(err);
            errorCode = err;
            return false;
        }
        return true;
    }

    GLenum SwapChainGL::GetGLInterval(IntervalType interval) {
        switch (interval) {
            case IntervalType::FIFO: return 1;
            case IntervalType::IMMEDIATE: return 0;
            case IntervalType::FIFO_RELAXED: return 1;
            case IntervalType::MAILBOX:return 1;
            default: return 1;
        }
    }

    bool SwapChainGL::IsValid() {
        return window != nullptr && glfwGetCurrentContext() != nullptr;
    }

    
    SwapChainExecuteResult SwapChainGL::ProcessSwapChainCreateImpl(BaseDevice& device, const SwapChainCreateParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        auto baseWindow = device.GetWindow();
        if (!baseWindow) {
            SwapChainCreateResult result;
            result.windowHandle = nullptr;
            result.width = 0;
            result.height = 0;
            result.swapChainSize = 0;
            result.actualSwapChainSize = 0;
            result.bufferCount = 0;
            result.intervalType = params.intervalType;
            result.isCreated = false;
            result.isInitialized = false;
            result.errorMessage = "BaseWindow is null";
            return SwapChainExecuteResult(result);
        }

        void* nativeHandle = std::any_cast<void*>(baseWindow->GetNativeHandle());
        window = GetWindow(nativeHandle);
        if (!window) {
            SwapChainCreateResult result;
            result.windowHandle = nativeHandle;
            result.width = 0;
            result.height = 0;
            result.swapChainSize = 0;
            result.actualSwapChainSize = 0;
            result.bufferCount = 0;
            result.intervalType = params.intervalType;
            result.isCreated = false;
            result.isInitialized = false;
            result.errorMessage = "Failed to get GLFWwindow from native handle";
            return SwapChainExecuteResult(result);
        }

        if (!glfwGetCurrentContext()) {
            glfwMakeContextCurrent(window);
            success = CheckGLError("glfwMakeContextCurrent", errorMsg, errorCode);
        }

        currentWidth = params.width;
        currentHeight = params.height;
        swapChainSize = params.swapChainSize > 0 ? params.swapChainSize : 2;
        currentInterval = params.intervalType;

        if (success) {
            glViewport(0, 0, currentWidth, currentHeight);
            success = CheckGLError("glViewport", errorMsg, errorCode);
        }

        if (success) {
            glfwSwapInterval(GetGLInterval(params.intervalType));
            CheckGLError("glfwSwapInterval", errorMsg, errorCode);
        }

        if (success) {
            glfwShowWindow(window);
        }

        SwapChainCreateResult result;
        result.windowHandle = nativeHandle;
        result.width = currentWidth;
        result.height = currentHeight;
        result.swapChainSize = params.swapChainSize;
        result.actualSwapChainSize = swapChainSize;
        result.bufferCount = swapChainSize;
        result.intervalType = currentInterval;
        result.isCreated = success;
        result.isInitialized = success;
        result.errorMessage = errorMsg;

        return SwapChainExecuteResult(result);
    }


    SwapChainExecuteResult SwapChainGL::ProcessSwapChainResizeImpl(BaseDevice& device, const SwapChainResizeParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        uint32_t oldWidth = currentWidth;
        uint32_t oldHeight = currentHeight;

        if (!window) {
            SwapChainResizeResult result;
            result.oldWidth = oldWidth;
            result.oldHeight = oldHeight;
            result.newWidth = 0;
            result.newHeight = 0;
            result.isResized = false;
            result.backBufferCount = 0;
            result.errorMessage = "Window is null";
            return SwapChainExecuteResult(result);
        }

        currentWidth = params.newWidth;
        currentHeight = params.newHeight;

        glViewport(0, 0, currentWidth, currentHeight);
        success = CheckGLError("glViewport", errorMsg, errorCode);

        SwapChainResizeResult result;
        result.oldWidth = oldWidth;
        result.oldHeight = oldHeight;
        result.newWidth = currentWidth;
        result.newHeight = currentHeight;
        result.isResized = success;
        result.backBufferCount = swapChainSize;
        result.errorMessage = errorMsg;

        return SwapChainExecuteResult(result);
    }


    SwapChainExecuteResult SwapChainGL::ProcessSwapChainPresentImpl(BaseDevice& device, const SwapChainPresentParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (!window) {
            SwapChainPresentResult result;
            result.syncInterval = params.syncInterval;
            result.intervalType = params.intervalType;
            result.isPresented = false;
            result.frameNumber = frameCounter;
            result.presentationTimeMs = 0.0;
            result.gpuTimeMs = 0.0;
            result.presentCounter = presentCounter;
            result.errorMessage = "Window is null";
            return SwapChainExecuteResult(result);
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        if (params.intervalType != currentInterval) {
            currentInterval = params.intervalType;
            glfwSwapInterval(GetGLInterval(params.intervalType));
            CheckGLError("glfwSwapInterval", errorMsg, errorCode);
        }

        glfwSwapBuffers(window);
        CheckGLError("glfwSwapBuffers", errorMsg, errorCode);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        double presentTimeMs = duration.count() / 1000.0;

        frameCounter++;
        presentCounter++;
        lastPresentTime = presentTimeMs;

        SwapChainPresentResult result;
        result.syncInterval = params.syncInterval;
        result.intervalType = currentInterval;
        result.isPresented = success;
        result.frameNumber = frameCounter;
        result.presentationTimeMs = presentTimeMs;
        result.gpuTimeMs = -1.0;
        result.presentCounter = presentCounter;
        result.errorMessage = errorMsg;
        
        return SwapChainExecuteResult(result);
    }


    SwapChainExecuteResult SwapChainGL::ProcessSwapChainDestroyImpl(BaseDevice& device, const SwapChainDestroyParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (!window) {
            SwapChainDestroyResult result;
            result.forceImmediate = params.forceImmediate;
            result.isDestroyed = false;
            result.resourcesFreed = false;
            result.freedMemoryBytes = 0;
            result.errorMessage = "Window is null";
            return SwapChainExecuteResult(result);
        }

        if (params.forceImmediate) {
            glfwDestroyWindow(window);
            window = nullptr;
            
            SwapChainDestroyResult result;
            result.forceImmediate = params.forceImmediate;
            result.isDestroyed = true;
            result.resourcesFreed = true;
            result.freedMemoryBytes = 0;
            result.errorMessage = "";
            return SwapChainExecuteResult(result);
        }

        glfwSetWindowShouldClose(window, GLFW_TRUE);
        
        CheckGLError("glfwSetWindowShouldClose", errorMsg, errorCode);

        SwapChainDestroyResult result;
        result.forceImmediate = params.forceImmediate;
        result.isDestroyed = success;
        result.resourcesFreed = success;
        result.freedMemoryBytes = 0;
        result.errorMessage = errorMsg;

        return SwapChainExecuteResult(result);
    }
}