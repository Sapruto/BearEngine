#include "Texture.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "stb_image.h" 

int Texture::GetChannelsFromFormat(GLenum format) {
    switch(format) {
        case GL_RED:   return 1;
        case GL_RG:    return 2;
        case GL_RGB:   return 3;
        case GL_RGBA:  return 4;
        default:       return 0;
    }
}

bool Texture::Load() {
    if (textureID != 0) Unload();
    
    unsigned char* image_data = stbi_load(
        GetPath().c_str(), &width, &height, &channels, 4
    );
    
    if (!image_data) return false;
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image_data);
    
    loaded = true;
    return true;
}

void Texture::Unload() {
    if (textureID != 0) {
        if (glfwGetCurrentContext() != nullptr) {
            glDeleteTextures(1, &textureID);
        }
        textureID = 0;
    }
}

void* Texture::GetImTextureID() const {
    return reinterpret_cast<void*>(static_cast<intptr_t>(textureID));
}

Texture::Size Texture::GetSize() const {
    return Size(static_cast<float>(width), static_cast<float>(height));
}

Texture* Texture::CreateFromData(unsigned char* data, int width, int height, GLenum format) {
    Texture* texture = new Texture();
    
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    texture->SetGluInt(id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    texture->SetWidth(width);
    texture->SetHeight(height);

    int channels = GetChannelsFromFormat(format);
    texture->SetChannels(channels);
    
    return texture;
}