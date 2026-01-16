#include "include/core/graphics_engine/Sprite.h"

#include "src/dependencies/imgui-master/imgui.h"

#include <dependencies/glad/include/glad/glad.h>
#include <dependencies/GLFW/include/GLFW/glfw3.h>
#include <cstdlib>

#include "src/dependencies/stb/stb_image.h"

#ifndef IMGUI_VERSION
    #define IM_ALLOC(size) malloc(size)
    #define IM_FREE(ptr) free(ptr)
#endif

Sprite::Sprite(GLFWwindow* window) : window(window), isLoaded(false), texture(0) {}

Sprite::~Sprite(){
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }    
}

void Sprite::LoadTextureFromMemory(const void* data, size_t data_size)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(
        (const unsigned char*)data, 
        (int)data_size, 
        &image_width, 
        &image_height, 
        NULL, 
        4
    );
    if (image_data == NULL)
        return;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image_data);

    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }

    texture = image_texture;
    width = image_width;
    height = image_height;

    isLoaded = true;
}

void Sprite::Start(){
    Component::Start();
}
void Sprite::Update(float deltaTime){
    Component::Update(deltaTime);
    if (!window) return;

    ImGui::Begin("OpenGL Texture Text");
    ImGui::Text("pointer = %x", texture);
    ImGui::Text("size = %d x %d", width, height);
    ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(width, height));
    ImGui::End();
}
void Sprite::Destroy() {
    Component::Destroy();
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
        isLoaded = false;
    }
}

void Sprite::LoadTextureFromFile(const char* file_name)
{
    FILE* file = fopen(file_name, "rb");
    if (file == NULL)
        return;
    fseek(file, 0, SEEK_END);

    long file_size_long = ftell(file);
    if (file_size_long <= 0) {
        fclose(file);
        return;
    }
    size_t file_size = (size_t)file_size_long;

    fseek(file, 0, SEEK_SET);

    void* file_data = IM_ALLOC(file_size);
    size_t bytes_read = fread(file_data, 1, file_size, file);
    if (bytes_read != file_size) {
        IM_FREE(file_data);
        fclose(file);  
        return;
    }

    fclose(file);

    LoadTextureFromMemory(file_data, file_size);

    IM_FREE(file_data);
}