#pragma once

#include "Image.h"

#include "Canvas.h"
#include "RectTransform.h"
#include "Vector3.h"
#include "Texture.h"
#include "UIRenderSettings.h"
#include "UIRect.h"

Image::Image(Texture* tex) : settings(tex) {}

void Image::CalculateGeometry() { 
    cachedVertices.clear();
    cachedIndices.clear();
    
    UIRect rect = rectTransform->GetScreenRect(canvas->GetScreenWidth(), canvas->GetScreenHeight());
    if (rect.width <= 0.0f || rect.height <= 0.0f) return; 
    
    auto& settings = GetRenderSettings();
    
    unsigned int baseIndex = static_cast<unsigned int>(cachedVertices.size());
    
    float u1 = settings.uvRect.x;
    float u2 = u1 + settings.uvRect.width;
    float v1 = settings.uvRect.y;
    float v2 = v1 + settings.uvRect.height;
    
    float x1 = rect.x;
    float x2 = x1 + rect.width;
    float y1 = rect.y;
    float y2 = y1 + rect.height;
    
    auto& c = settings.color;
    
    cachedVertices.push_back({x1, y1, u1, v1, c.r, c.g, c.b, c.a});
    cachedVertices.push_back({x2, y1, u2, v1, c.r, c.g, c.b, c.a});
    cachedVertices.push_back({x2, y2, u2, v2, c.r, c.g, c.b, c.a});
    cachedVertices.push_back({x1, y2, u1, v2, c.r, c.g, c.b, c.a});
    
    cachedIndices.push_back(baseIndex + 0);
    cachedIndices.push_back(baseIndex + 1);
    cachedIndices.push_back(baseIndex + 2);
    cachedIndices.push_back(baseIndex + 0);
    cachedIndices.push_back(baseIndex + 2);
    cachedIndices.push_back(baseIndex + 3);
}

Texture* Image::GetTexture() { 
    return settings.texture; 
}

glm::vec4 Image::GetColor() { 
    return settings.color; 
}

void Image::SetTexture(Texture* texture) { 
    settings.texture = texture; 
}

void Image::SetColor(glm::vec4 color) { 
    settings.color = color; 
}

const UIRenderSettings& Image::GetRenderSettings() const{
    return settings;
}