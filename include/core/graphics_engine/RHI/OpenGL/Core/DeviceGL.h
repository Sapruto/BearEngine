#pragma once

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/OpenGL/Core/WindowGL.h"

class OpenGLDevice : public BaseDevice {
private:
    WindowGL* GetGLWindow() const {
        return static_cast<WindowGL*>(window.get());
    }

public:
    explicit OpenGLDevice(std::shared_ptr<WindowGL> win)
        : BaseDevice(win) {}
    
    bool Initialize() override {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return false;
        }

        auto glWindow = GetGLWindow();
        if (!glWindow || !glWindow->GetGLFWWindow()) {
            return false;
        }
        
        glfwMakeContextCurrent(glWindow->GetGLFWWindow());
        nativeDevice = glfwGetCurrentContext();
        
        return true;
    }
    
    void Shutdown() override {

    }
    
    void Present() override {
        auto glWindow = GetGLWindow();
        if (glWindow) {
            glWindow->SwapBuffers();
        }
    }

    void Clear(ClearFlagsRHI flags = ClearFlagsRHI::All) override {
        GLbitfield mask = 0;

        if (HasFlag(flags, ClearFlagsRHI::Color)) {
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            mask |= GL_COLOR_BUFFER_BIT;
        }
        if (HasFlag(flags, ClearFlagsRHI::Depth)) {
            glClearDepth(clearDepthValue);
            mask |= GL_DEPTH_BUFFER_BIT;
        }
        if (HasFlag(flags, ClearFlagsRHI::Stencil)) {
            glClearStencil(clearStencilValue);
            mask |= GL_STENCIL_BUFFER_BIT;
        }

        if (mask != 0) {
            glClear(mask);
        }
    }
    
    GLFWwindow* GetGLFWWindow() const {
        auto glWindow = GetGLWindow();
        return glWindow ? glWindow->GetGLFWWindow() : nullptr;
    }
};