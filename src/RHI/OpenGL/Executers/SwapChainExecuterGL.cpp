#include "RHI/OpenGL/Executors/SwapChainExecutorGL.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ResourceGL.h"
#include "RHI/OpenGL/HandlesGL.h"
#include "RHI/OpenGL/Utilities.h"

using namespace RHI::Base;
using namespace RHI::Executors::SwapChainExecutor;
using namespace RHI::Handles;
using namespace RHI::OpenGL;
using namespace RHI::OpenGL::Utilities;

SwapChainExecuteResult SwapChainExecutorGL::ProcessCreateSwapChainImpl(BaseDevice& device, const SwapChainCreateParams& params) {
    SwapChainExecuteResult result;

    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    window = deviceGL.GetGLFWWindow();

    if (!window) {
        return SwapChainExecuteResult(SwapChainCreateResult{
            params.width, params.height, params.bufferCount,
            params.presentMode, params.colorSpace,
            false, "Failed to get GLFWwindow"
        });
    }

    if (!glfwGetCurrentContext()) {
        glfwMakeContextCurrent(window);
    }

    currentWidth = params.width;
    currentHeight = params.height;
    currentBufferCount = params.bufferCount > 0 ? params.bufferCount : 2;
    currentPresentMode = params.presentMode;
    currentColorSpace = params.colorSpace;

    glViewport(0, 0, currentWidth, currentHeight);
    glfwSwapInterval(ToGLInterval(params.presentMode));
    glfwShowWindow(window);

    return SwapChainExecuteResult(SwapChainCreateResult{
        currentWidth, currentHeight, currentBufferCount,
        currentPresentMode, currentColorSpace,
        true, ""
    });
}

SwapChainExecuteResult SwapChainExecutorGL::ProcessResizeSwapChainImpl([[maybe_unused]] BaseDevice& device, const SwapChainResizeParams& params) {
    uint32_t oldWidth = currentWidth;
    uint32_t oldHeight = currentHeight;

    if (!window) {
        return SwapChainExecuteResult(SwapChainResizeResult{
            oldWidth, oldHeight, 0, 0, currentBufferCount,
            false, "Window is null"
        });
    }

    currentWidth = params.newWidth;
    currentHeight = params.newHeight;
    glViewport(0, 0, currentWidth, currentHeight);

    return SwapChainExecuteResult(SwapChainResizeResult{
        oldWidth, oldHeight, currentWidth, currentHeight,
        currentBufferCount, true, ""
    });
}

SwapChainExecuteResult SwapChainExecutorGL::ProcessPresentSwapChainImpl(
        [[maybe_unused]] BaseDevice& device,
        const SwapChainPresentParams& params) {

    if (!window) {
        return SwapChainExecuteResult(SwapChainPresentResult{
            params.presentMode, frameCounter, false, "Window is null"
        });
    }

    if (params.presentMode != currentPresentMode) {
        currentPresentMode = params.presentMode;
        glfwSwapInterval(ToGLInterval(params.presentMode));
    }

    if (params.renderTarget.id != 0) {
        DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
        ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

        auto* rt = resourceGL.GetNativeAs<RenderTargetGL>(params.renderTarget);
        if (rt) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(
                0, 0, rt->width, rt->height,
                0, 0, currentWidth, currentHeight,
                GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    glfwSwapBuffers(window);
    frameCounter++;

    return SwapChainExecuteResult(SwapChainPresentResult{
        currentPresentMode, frameCounter, true, ""
    });
}

SwapChainExecuteResult SwapChainExecutorGL::ProcessDestroySwapChainImpl([[maybe_unused]] BaseDevice& device, const SwapChainDestroyParams& params) {
    if (!window) {
        return SwapChainExecuteResult(SwapChainDestroyResult{
            false, "Window is null"
        });
    }

    if (params.forceImmediate) {
        glfwDestroyWindow(window);
        window = nullptr;
        return SwapChainExecuteResult(SwapChainDestroyResult{ true, "" });
    }

    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return SwapChainExecuteResult(SwapChainDestroyResult{ true, "" });
}

bool SwapChainExecutorGL::IsValid() {
    return glfwGetCurrentContext() != nullptr;
}