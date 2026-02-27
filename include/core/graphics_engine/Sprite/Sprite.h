#pragma once

#include <string>
#include "include/core/system_engine/component_system/Component.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Texture.h"

#include "include/core/graphics_engine/BaseGraphic/RenderComponent.h"

class SpriteRenderer;

class Sprite : public RenderComponent {
private:
    SpriteRenderer* renderer;

    std::string texturePath;
    Texture* texture = nullptr;

    float width = 100.0f;
    float height = 100.0f;
    
    glm::vec4 color = glm::vec4(1.0f);

    bool isActive = true;

    int layer = 0;
    
    void LoadTexture();
    
public:
    Sprite(SpriteRenderer& renderer, const std::string& texturePath = "");
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
    
    void Render(const glm::vec2& position, float rotation);
    
    void RenderAtPosition(float screenX, float screenY, 
                         float width = 0, float height = 0);

    void SetRender(SpriteRenderer& r);

    void ClearRender();

    bool IsActive() const { return isActive; } 
    bool IsVisible() const override { return isActive; }
    int GetLayer() const override { return layer; }

    float GetWidth() const { return width; }  
    float GetHeight() const { return height; } 
    glm::vec4 GetColor() const { return color; } 
};