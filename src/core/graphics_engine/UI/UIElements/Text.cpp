#include "Text.h"
#include <iostream>

void Text::CalculateGeometry() {
    cachedVertices.clear();
    cachedIndices.clear();
    
    if (!font || text.empty() || !canvas) return;
    
    settings.texture = const_cast<Texture*>(font->GetAtlas());
    settings.color = color;
    
    UIRect rect = rectTransform->GetScreenRect(canvas->GetScreenWidth(), canvas->GetScreenHeight());
    
    float startX = rect.x;
    float startY = rect.y + rect.height - font->GetSize();
    float x = startX;
    float y = startY;
    
    float scale = 1.0f;
    
    for (char c : text) {
        if (c == ' ') {
            x += font->GetGlyphData('A').advance * scale * 0.5f;
            continue;
        }
        
        const GlyphData& glyph = font->GetGlyphData(c);
        
        float x1 = x + glyph.bearing.x * scale;
        float y1 = y - (glyph.size.y - glyph.bearing.y) * scale;
        float x2 = x1 + glyph.size.x * scale;
        float y2 = y1 + glyph.size.y * scale;
        
        float u1 = glyph.uvRect.x;
        float v1 = glyph.uvRect.y;
        float u2 = glyph.uvRect.x + glyph.uvRect.width;
        float v2 = glyph.uvRect.y + glyph.uvRect.height;
        
        unsigned int baseIndex = cachedVertices.size();
        const auto& col = settings.color;
        
        cachedVertices.push_back({x1, y1, u1, v1, col.r, col.g, col.b, col.a});
        cachedVertices.push_back({x2, y1, u2, v1, col.r, col.g, col.b, col.a});
        cachedVertices.push_back({x2, y2, u2, v2, col.r, col.g, col.b, col.a});
        cachedVertices.push_back({x1, y2, u1, v2, col.r, col.g, col.b, col.a});
        
        cachedIndices.push_back(baseIndex + 0);
        cachedIndices.push_back(baseIndex + 1);
        cachedIndices.push_back(baseIndex + 2);
        cachedIndices.push_back(baseIndex + 0);
        cachedIndices.push_back(baseIndex + 2);
        cachedIndices.push_back(baseIndex + 3);
        
        x += glyph.advance * scale;
    }
}