#pragma once

#include "Resource.h"

#include <glad/glad.h>
#include <string>

using GLuint = unsigned int;

class Texture : public Resource {
private:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;

    static int GetChannelsFromFormat(GLenum format);

public:
    Texture() = default;
    Texture(const std::string& path) : Resource(path) {}
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

    void SetWidth(int width) {this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetChannels(int channels) { this->channels = channels; }
    void SetGluInt(GLuint textureID) { this->textureID = textureID; }
    
    Size GetSize() const;

    static Texture* CreateFromData(unsigned char* data, int width, int height, GLenum format=GL_RED);
};