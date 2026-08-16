#pragma once

#include <array>

#include <GLFW/glfw3.h>

#include "Keys.h"
#include <string>

enum class KeyState {
    Released = 0,
    Pressed,
    JustPressed,
    JustReleased  
};

class InputSystem {
private:
    static InputSystem* instance;

    static constexpr int KEY_COUNT = GLFW_KEY_LAST + 1;
    static constexpr int MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;
    
    GLFWwindow* window;
    
    std::array<KeyState, KEY_COUNT> keyStates;
    std::array<KeyState, MOUSE_BUTTON_COUNT> mouseButtonStates;
    
    double mouseX, mouseY;
    double lastMouseX, lastMouseY;
    double mouseScrollX, mouseScrollY;
    
    std::string textInputBuffer;

    void BeginUpdateMouse();
    void BeginUpdateKeys();

    void EndUpdateMouse();
    void EndUpdateKeys();
    
    InputSystem();
    
public:
    static InputSystem& GetInstance();
    
    void Initialize(GLFWwindow* window);
    
    void BeginUpdate();
    void EndUpdate();
    
    bool GetKey(Keys key);
    bool GetKeyDown(Keys key); 
    bool GetKeyUp(Keys key);   
    
    bool GetMouseButton(int button);
    bool GetMouseButtonDown(int button);
    bool GetMouseButtonUp(int button);
    
    double GetMouseX();
    double GetMouseY();

    double GetMouseScrollX() { return mouseScrollX; }
    double GetMouseScrollY() { return mouseScrollY; }
    
private:
    KeyState& GetKeyState(int keyCode);
    KeyState& GetMouseButtonState(int button);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#define UpdateInputSystem(name, ...) void name() { \
    InputSystem::GetInstance().BeginUpdate(); \
    __VA_ARGS__ \
    InputSystem::GetInstance().EndUpdate(); \
}