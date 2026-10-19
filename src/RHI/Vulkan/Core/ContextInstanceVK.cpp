#include "RHI/Vulkan/Core/ContextInstanceVK.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <iostream>

using namespace RHI::Vulkan;

ContextInstanceVK::ContextInstanceVK
() {
    glfwSetErrorCallback([](int code, const char* msg) {
        std::cerr << "GLFW " << code << ": " << msg << "\n";
    });
}

bool ContextInstanceVK::Initialize() {
    if (initialized) return true;

    if (!glfwInit()) {
        std::cerr << "RHIContextGL glfwInit failed\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    initialized = true;
    return true;
}

void ContextInstanceVK::ChangeGLHints(int newMajor, int newMinor) {
    glMajor = newMajor;
    glMinor = newMinor;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
}

void ContextInstanceVK::Shutdown() {
    if (!initialized) return;
    glfwTerminate();
    initialized = false;
}