#include "Sprite.h"
#include "SpriteRenderer.h"
#include "Transform2D.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include <iostream>

Sprite::Sprite(ResourceManager& rm, SpriteRenderer& r, const std::string& texturePath) 
    : rm(&rm), renderer(&r), texturePath(texturePath)
{
    LoadTexture();

    renderer->RegisterRenderComponent(this);
}

Sprite::~Sprite() {
    if (renderer) {
        renderer->UnregisterRenderComponent(this);
    }
}

void Sprite::SetRender(SpriteRenderer& r) {
    renderer = &r;
}


void Sprite::SetResourceManager(ResourceManager& r) {
    rm = &r;
}

void Sprite::ClearRender() {
    renderer = nullptr;
}
void Sprite::ClearResourceManager() {
    rm = nullptr;
}

void Sprite::LoadTexture() {
    if (texturePath.empty()) return;
    
    rm->LoadResource(texturePath, ResourceType::Texture);
    texture = static_cast<Texture*>(rm->GetResource(texturePath));
    
    if (texture && texture->IsLoaded()) {
        width = static_cast<float>(texture->GetWidth());
        height = static_cast<float>(texture->GetHeight());
    }
}

void Sprite::SetTexture(const std::string& path) {
    texturePath = path;
    LoadTexture();
}

void Sprite::RenderAtPosition(float screenX, float screenY,
                             float w, float h) {
    if (!texture || !texture->IsLoaded()) return;
    
    float finalWidth = (width > 0) ? width : this->width;
    float finalHeight = (height > 0) ? height : this->height;
    
    renderer->RenderSprite(
        texture->GetOpenGLTextureID(),
        screenX, screenY,
        finalWidth, finalHeight,
        0.0f,
        color.r, color.g, color.b, color.a
    );
}

void Sprite::SetScale(float scaleX, float scaleY) {
    if (texture && texture->IsLoaded()) {
        width = texture->GetWidth() * scaleX;
        height = texture->GetHeight() * scaleY;
    } else {
        width = 100.0f * scaleX;
        height = 100.0f * scaleY;
    }
}