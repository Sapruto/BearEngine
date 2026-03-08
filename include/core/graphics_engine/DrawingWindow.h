#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream> 

class DrawingManager {
private:
    GLFWwindow* window;
    int window_x;
    int window_y;
    float bg_color[4];
    const char* title;
    
public:
    DrawingManager(int window_x, int window_y, float clear_color[4], const char* title);
    
    ~DrawingManager();

    bool Initialize();
    
    void Clear();
    
    void SwapBuffers();
    
    void PollEvents();
    
    bool ShouldClose();
    
    GLFWwindow* GetWindow() const;
};