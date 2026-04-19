#pragma once

#include <string>
#include "Component.h"
#include "Texture.h"

#include "RenderComponent.h"

class ResourceManager;
class SpriteRenderer;

class Sprite : public RenderComponent {
private:
    ResourceManager* rm;
    SpriteRenderer* renderer;

    std::string texturePath;
    Texture* texture = nullptr;

    float width = 100.0f;
    float height = 100.0f;
    
    glm::vec4 color = glm::vec4(1.0f);
    
    void LoadTexture();
    
public:
    Sprite(ResourceManager& rm, SpriteRenderer& r, const std::string& texturePath);
    ~Sprite();
    
    void SetTexture(const std::string& path);
    Texture* GetTexture() const { return texture; }
    
    void SetSize(float width, float height) { 
        this->width = width; 
        this->height = height; 
    }
    
    void SetScale(float scaleX, float scaleY);
    
    void SetColor(float r, float g, float b, float a = 1.0f) {
        color = glm::vec4(r, g, b, a);
    }
    
    void RenderAtPosition(float screenX, float screenY, 
                         float width = 0, float height = 0);

    void SetRender(SpriteRenderer& r);
    void SetResourceManager(ResourceManager& r);

    void ClearRender();
    void ClearResourceManager();

    bool IsActive() const { return isActive; } 

    float GetWidth() const { return width; }  
    float GetHeight() const { return height; } 
    glm::vec4 GetColor() const { return color; } 
};