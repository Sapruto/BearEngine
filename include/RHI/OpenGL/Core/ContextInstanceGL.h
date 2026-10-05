#pragma once

#include "RHI/Base/Core/BaseContextInstanceRHI.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

namespace RHI::OpenGL {
    class ContextInstanceGL : public Base::BaseContextInstanceRHI {
    private:
        int glMajor{3};
        int glMinor{3};

    public:
        ContextInstanceGL() = default;
        ~ContextInstanceGL() override = default;

        bool Initialize() override {
            if (initialized) return true;

            glfwSetErrorCallback([](int code, const char* msg) {
                std::cerr << "GLFW " << code << ": " << msg << "\n";
            });

            if (!glfwInit()) {
                std::cerr << "RHIContextGL glfwInit failed\n";
                return false;
            }

            glMajor = 3;
            glMinor = 3;
            vsync = true;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            initialized = true;
            return true;
        }

        void Shutdown() override {
            if (!initialized) return;
            glfwTerminate();
            initialized = false;
        }
    };
}