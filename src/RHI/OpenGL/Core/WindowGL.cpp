#include "RHI/OpenGL/Core/WindowGL.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <iostream>

using namespace RHI::OpenGL;

void WindowGL::SetupCallbacks() {
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
}

void WindowGL::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowGL* self = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));
    if (self) {
        glViewport(0, 0, width, height);
        self->SetSize(width, height);
    }
}

void WindowGL::WindowCloseCallback(GLFWwindow* window) {
    WindowGL* self = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));
    if (self && self->closeCallback) {
        self->closeCallback();
    }
}

void WindowGL::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowGL* self = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));
    if (self && key >= 0 && key < 256) {
        self->keys[key] = (action != GLFW_RELEASE);
    }
}

void WindowGL::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    WindowGL* self = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->mousePos.x = static_cast<float>(xpos);
        self->mousePos.y = static_cast<float>(ypos);
    }
}

bool WindowGL::Create(const std::string& title, int w, int h) {
    this->title = title;
    
    window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!window) {
        const char* desc;
        glfwGetError(&desc);
        std::cerr << "Failed to create GLFW window: " << desc << std::endl;
        return false;
    }
    
    nativeHandle = window;
    SetupCallbacks();
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    SetSize(fbWidth, fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSetWindowUserPointer(window, this);
    
    return true;
}

void WindowGL::Destroy() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
}

void WindowGL::PollEvents() {
    glfwPollEvents();
}

void WindowGL::SetTitle(const std::string& title) {
    if (window) {
        glfwSetWindowTitle(window, title.c_str());
    }
}

bool WindowGL::ShouldClose() const {
    return window ? glfwWindowShouldClose(window) : true;
}