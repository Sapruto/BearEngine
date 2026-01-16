#pragma once

#include "include/core/Component.h"

#include <dependencies/glad/include/glad/glad.h>
#include <dependencies/GLFW/include/GLFW/glfw3.h>
#include "src/dependencies/imgui-master/imgui.h"

#include "src/dependencies/stb/stb_image.h"

class Sprite : public Component{
private:
    int width = 0;
    int height = 0;

    GLFWwindow* window;
    GLuint texture = 0;

    bool isLoaded;

    void LoadTextureFromMemory(const void* data, size_t data_size);
public:
    Sprite(GLFWwindow* window);
    ~Sprite();

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override; 

    void LoadTextureFromFile(const char* file_name);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool IsLoaded() const { return isLoaded; }
    GLuint GetTextureID() const { return texture; }
};