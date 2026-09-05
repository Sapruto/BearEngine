#include "Text.h"
#include <iostream>
#include <cstring>

void Text::CalculateGeometry() {
    cachedVertices.clear();
    cachedIndices.clear();
    
    if (!font || text.empty() || !canvas) return;
    
    settings.texture = const_cast<Texture*>(font->GetAtlas());
    
    UIRect rect = rectTransform->GetScreenRect(canvas->GetScreenWidth(), canvas->GetScreenHeight());
    
    float startX = rect.x;
    float startY = rect.y + rect.height - font->GetSize();
    float x = startX;
    float y = startY;
    
    float scale = 1.0f;
    
    float col[4];
    col[0] = settings.color[0];
    col[1] = settings.color[1];
    col[2] = settings.color[2];
    col[3] = settings.color[3];
    
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
        
        cachedVertices.push_back({x1, y1, u1, v1, col[0], col[1], col[2], col[3]});
        cachedVertices.push_back({x2, y1, u2, v1, col[0], col[1], col[2], col[3]});
        cachedVertices.push_back({x2, y2, u2, v2, col[0], col[1], col[2], col[3]});
        cachedVertices.push_back({x1, y2, u1, v2, col[0], col[1], col[2], col[3]});
        
        cachedIndices.push_back(baseIndex + 0);
        cachedIndices.push_back(baseIndex + 1);
        cachedIndices.push_back(baseIndex + 2);
        cachedIndices.push_back(baseIndex + 0);
        cachedIndices.push_back(baseIndex + 2);
        cachedIndices.push_back(baseIndex + 3);
        
        x += glyph.advance * scale;
    }
}