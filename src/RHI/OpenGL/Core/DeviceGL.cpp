#include "RHI/OpenGL/Core/DeviceGL.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/ResourceGL.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace RHI::Base;
using namespace RHI::OpenGL;

WindowGL* DeviceGL::GetGLWindow() const {
    return static_cast<WindowGL*>(window.get());
}

DeviceGL::DeviceGL(std::shared_ptr<WindowGL> win)
    : BaseDevice(win, std::make_shared<ResourceGL>()) {}

bool DeviceGL::Initialize() {
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

void DeviceGL::Shutdown() {

}

GLFWwindow* DeviceGL::GetGLFWWindow() const {
    auto glWindow = GetGLWindow();
    return glWindow ? glWindow->GetGLFWWindow() : nullptr;
}