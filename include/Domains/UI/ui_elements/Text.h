#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "UIElement.h"
#include "Font.h"
#include "Canvas.h"
#include "UIRenderSettings.h"

class Text : public UIElement {
private:
    Font* font = nullptr;
    std::string text;
    UIRenderSettings settings;

    void CalculateGeometry() override;

public:
    Text() {
        settings.color[0] = 1.0f;
        settings.color[1] = 1.0f;
        settings.color[2] = 1.0f;
        settings.color[3] = 1.0f;
    }

    Text(Font& font, const std::string& text) 
        : font(&font), text(text) {
        settings.texture = const_cast<Texture*>(font.GetAtlas());
        settings.color[0] = 1.0f;
        settings.color[1] = 1.0f;
        settings.color[2] = 1.0f;
        settings.color[3] = 1.0f;
    }
        
    const UIRenderSettings& GetRenderSettings() const override { return settings; }
    
    void SetText(const std::string& newText) { 
        text = newText; 
        CalculateGeometry();
    }
    
    void SetFont(Font& newFont) {
        font = &newFont; 
        settings.texture = const_cast<Texture*>(newFont.GetAtlas());
    }

    void SetColor(const float color[4]) {
        settings.color[0] = color[0];
        settings.color[1] = color[1];
        settings.color[2] = color[2];
        settings.color[3] = color[3];
    }

    void GetColor(float out[4]) const {
        out[0] = settings.color[0];
        out[1] = settings.color[1];
        out[2] = settings.color[2];
        out[3] = settings.color[3];
    }
    
    const std::string& GetText() const { return text; }
    Font* GetFont() const { return font; } 

    void SetFontSize(int size) { font->SetFontSize(size); }
};