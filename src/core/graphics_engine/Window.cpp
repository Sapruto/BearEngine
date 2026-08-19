#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <iostream> 

Window::Window(int window_x, int window_y, float clear_color[4], const char* title) 
    : window(nullptr), window_x(window_x), window_y(window_y), title(title) {
    
    for (int i = 0; i < 4; i++) {
        this->bg_color[i] = clear_color[i];
    }
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::Initialize() {
    if (!glfwInit()) {
        std::cout << "GLFW init error\n";
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(window_x, window_y, title, NULL, NULL);
    if (!window) {
        std::cout << "GLFW create window error\n";
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD load GL error\n";
        return false;
    }
    
    glViewport(0, 0, window_x, window_y);
    
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
    });
    
    return true;
}

void Window::Clear() {
    glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers() {
    glfwSwapBuffers(window);
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

GLFWwindow* Window::GetWindow() const {
    return window;
}