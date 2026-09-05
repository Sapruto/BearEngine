#include "Image.h"
#include "Canvas.h"
#include "RectTransform.h"
#include "UIRenderSettings.h"

Image::Image() {
    settings = UIRenderSettings();
}

Image::Image(Texture* tex) {
    UIRenderSettings s;
    s.texture = tex;
    settings = s;
}

void Image::CalculateGeometry() { 
    cachedVertices.clear();
    cachedIndices.clear();
    
    UIRect rect = rectTransform->GetScreenRect(canvas->GetScreenWidth(), canvas->GetScreenHeight());
    if (rect.width <= 0.0f || rect.height <= 0.0f) return; 
    
    const UIRenderSettings& s = GetRenderSettings();
    
    unsigned int baseIndex = static_cast<unsigned int>(cachedVertices.size());
    
    float u1 = s.uvRect.x;
    float u2 = u1 + s.uvRect.width;
    float v1 = s.uvRect.y;
    float v2 = v1 + s.uvRect.height;
    
    float x1 = rect.x;
    float x2 = x1 + rect.width;
    float y1 = rect.y;
    float y2 = y1 + rect.height;
    
    const float* c = s.color;
    
    cachedVertices.push_back({x1, y1, u1, v1, c[0], c[1], c[2], c[3]});
    cachedVertices.push_back({x2, y1, u2, v1, c[0], c[1], c[2], c[3]});
    cachedVertices.push_back({x2, y2, u2, v2, c[0], c[1], c[2], c[3]});
    cachedVertices.push_back({x1, y2, u1, v2, c[0], c[1], c[2], c[3]});
    
    cachedIndices.push_back(baseIndex + 0);
    cachedIndices.push_back(baseIndex + 1);
    cachedIndices.push_back(baseIndex + 2);
    cachedIndices.push_back(baseIndex + 0);
    cachedIndices.push_back(baseIndex + 2);
    cachedIndices.push_back(baseIndex + 3);
}

Texture* Image::GetTexture() { 
    return settings.GetValue().texture; 
}

const float* Image::GetColor() const { 
    return settings.GetValue().color; 
}

void Image::GetColor(float out[4]) const {
    const float* c = settings.GetValue().color;
    out[0] = c[0];
    out[1] = c[1];
    out[2] = c[2];
    out[3] = c[3];
}

void Image::SetTexture(Texture* texture) { 
    settings.GetValue().texture = texture; 
}

void Image::SetColor(const float color[4]) {
    float* c = settings.GetValue().color;
    c[0] = color[0];
    c[1] = color[1];
    c[2] = color[2];
    c[3] = color[3];
}

const UIRenderSettings& Image::GetRenderSettings() const {
    return settings.GetValue();
}