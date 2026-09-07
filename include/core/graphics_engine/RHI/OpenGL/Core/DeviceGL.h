#pragma once

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/OpenGL/Core/OpenGLWindow.h"

class OpenGLDevice : public BaseDevice {
private:
    OpenGLWindow* GetGLWindow() const {
        return static_cast<OpenGLWindow*>(window.get());
    }

public:
    explicit OpenGLDevice(std::shared_ptr<OpenGLWindow> win) 
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
    
    GLFWwindow* GetGLFWWindow() const {
        auto glWindow = GetGLWindow();
        return glWindow ? glWindow->GetGLFWWindow() : nullptr;
    }
};