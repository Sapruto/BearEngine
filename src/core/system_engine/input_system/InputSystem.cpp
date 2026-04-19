#include "InputSystem.h"

#include <iostream>
#include <string>

#include "Keys.h"

InputSystem* InputSystem::instance = nullptr;

InputSystem::InputSystem() {
    keyStates.fill(KeyState::Released);
    mouseButtonStates.fill(KeyState::Released);
    mouseX = mouseY = 0;
    lastMouseX = lastMouseY = 0;
    mouseScrollX = mouseScrollY = 0;
}

InputSystem& InputSystem::GetInstance() {
    if (!instance) {
        instance = new InputSystem();
    }
    return *instance;
}

void InputSystem::Initialize(GLFWwindow* window) {
    if (!window) return;
    
    this->window = window;
    
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, MousePositionCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
    
    glfwGetCursorPos(window, &mouseX, &mouseY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void InputSystem::Update() {
    glfwPollEvents();
    
    if (window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mouseX = xpos;
        mouseY = ypos;
    }
    
    for (int i = 0; i < KEY_COUNT; i++) {
        KeyState& state = keyStates[i];
        if (state == KeyState::JustPressed) {
            state = KeyState::Pressed;
        }
        else if (state == KeyState::JustReleased) {
            state = KeyState::Released;
        }
    }
    
    for (int i = 0; i < MOUSE_BUTTON_COUNT; i++) {
        KeyState& state = mouseButtonStates[i];
        if (state == KeyState::JustPressed) {
            state = KeyState::Pressed;
        }
        else if (state == KeyState::JustReleased) {
            state = KeyState::Released;
        }
    }
    
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseScrollX = mouseScrollY = 0;
}

bool InputSystem::GetKey(Keys key) {
    KeyState state = GetInstance().keyStates[static_cast<int>(key)];
    return state == KeyState::Pressed || state == KeyState::JustPressed;
}

bool InputSystem::GetKeyDown(Keys key) {
    return GetInstance().keyStates[static_cast<int>(key)] == KeyState::JustPressed;
}

bool InputSystem::GetKeyUp(Keys key) {
    return GetInstance().keyStates[static_cast<int>(key)] == KeyState::JustReleased;
}

bool InputSystem::GetMouseButton(int button) {
    if (button < 0 || button >= MOUSE_BUTTON_COUNT) return false;
    KeyState state = GetInstance().mouseButtonStates[button];
    return state == KeyState::Pressed || state == KeyState::JustPressed;
}
bool InputSystem::GetMouseButtonDown(int button) {
    if (button < 0 || button >= MOUSE_BUTTON_COUNT) return false;
    return GetInstance().mouseButtonStates[button] == KeyState::JustPressed;
}
bool InputSystem::GetMouseButtonUp(int button) {
    if (button < 0 || button >= MOUSE_BUTTON_COUNT) return false;
    return GetInstance().mouseButtonStates[button] == KeyState::JustReleased;
}

KeyState& InputSystem::GetKeyState(int keyCode) {
    return GetInstance().keyStates[keyCode];
}

KeyState& InputSystem::GetMouseButtonState(int button) {
    return GetInstance().mouseButtonStates[button];
}


double InputSystem::GetMouseX() {
    return GetInstance().mouseX;
}

double InputSystem::GetMouseY() {
    return GetInstance().mouseY;
}

void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputSystem& input = InputSystem::GetInstance();
    
    if (key < 0 || key >= KEY_COUNT) return;
    
    switch (action) {
        case GLFW_PRESS:
            input.keyStates[key] = KeyState::JustPressed;
            break;
        case GLFW_RELEASE:
            input.keyStates[key] = KeyState::JustReleased;
            break;
        case GLFW_REPEAT:
            break;
    }
}

void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    InputSystem& input = InputSystem::GetInstance();
    
    if (button < 0 || button >= MOUSE_BUTTON_COUNT) return;
    
    switch (action) {
        case GLFW_PRESS:
            input.mouseButtonStates[button] = KeyState::JustPressed;
            break;
        case GLFW_RELEASE:
            input.mouseButtonStates[button] = KeyState::JustReleased;
            break;
    }
}
 
void InputSystem::MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    InputSystem& input = InputSystem::GetInstance();
    input.mouseX = xpos;
    input.mouseY = ypos;
}

void InputSystem::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    InputSystem& input = InputSystem::GetInstance();
    input.mouseScrollX = xoffset;
    input.mouseScrollY = yoffset;
}