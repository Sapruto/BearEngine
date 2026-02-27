#include "include/core/system_engine/resource_system/ResourcesTypes/Texture.h"
#include "src/dependencies/imgui-master/imgui.h"
#include <dependencies/glad/include/glad/glad.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "src/dependencies/stb/stb_image.h" 

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
        glDeleteTextures(1, &textureID);
        textureID = 0;
        loaded = false;
    }
}

void* Texture::GetImTextureID() const {
    return reinterpret_cast<void*>(static_cast<intptr_t>(textureID));
}

Texture::Size Texture::GetSize() const {
    return Size(static_cast<float>(width), static_cast<float>(height));
}