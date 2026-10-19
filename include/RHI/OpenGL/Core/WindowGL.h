#pragma once

#include "RHI/Base/Core/BaseWindow.h"

struct GLFWwindow;

namespace RHI::OpenGL {
    class WindowGL : public Base::BaseWindow {
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
        
        GLFWwindow* GetGLFWWindow() const { return window; }

        void SetTitle(const std::string& title) override;

        bool ShouldClose() const override;
    };
}