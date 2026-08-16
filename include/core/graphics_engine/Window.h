#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream> 

class Window {
private:
    GLFWwindow* window;
    int window_x;
    int window_y;
    float bg_color[4];
    const char* title;
    
public:
    Window(int window_x, int window_y, float clear_color[4], const char* title);
    
    ~Window();

    bool Initialize();
    
    void Clear();
    
    void SwapBuffers();
    
    void PollEvents();
    
    bool ShouldClose();

    int GetWidth() { return window_x; }
    int GetHeight() { return window_y; }
    
    GLFWwindow* GetWindow() const;
};