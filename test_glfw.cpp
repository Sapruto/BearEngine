#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
    
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW" << std::endl;
        return 1;
    }
    std::cout << "GLFW init OK" << std::endl;
    
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", NULL, NULL);
    if (!window) {
        const char* desc;
        int code = glfwGetError(&desc);
        std::cout << "Failed to create window: " << desc << " (code: " << code << ")" << std::endl;
        glfwTerminate();
        return 1;
    }
    std::cout << "Window created OK" << std::endl;
    
    glfwMakeContextCurrent(window);
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "Test passed!" << std::endl;
    return 0;
}
