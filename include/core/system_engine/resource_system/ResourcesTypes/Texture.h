#pragma once

#include "include/core/system_engine/resource_system/Resource.h"
#include <string>

using GLuint = unsigned int;

class Texture : public Resource {
private:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;

public:
    explicit Texture(const std::string& path) : Resource(path) {}
    ~Texture() override { Unload(); }

    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override { return textureID != 0; }
    
    GLuint GetTextureID() const { return textureID; }
    void* GetImTextureID() const;
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }

    GLuint GetOpenGLTextureID() const { 
        return textureID; 
    }
    
    struct Size {
        float x, y;
        Size(float x = 0, float y = 0) : x(x), y(y) {}
    };
    
    Size GetSize() const;
};