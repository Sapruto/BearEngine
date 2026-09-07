#pragma once

#include "RHI/Base/Core/BaseWindow.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class WindowGL : public BaseWindow {
private:
    GLFWwindow* window{nullptr};
    std::string title;
    
    void SetupCallbacks();
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void WindowCloseCallback(GLFWwindow* window);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

public:
    WindowGL() = default;
    ~WindowGL() override { Destroy(); }
    
    bool Create(const std::string& title, int w, int h) override;
    void Destroy() override;
    void PollEvents() override;
    void SwapBuffers() override;
    
    GLFWwindow* GetGLFWWindow() const { return window; }
};