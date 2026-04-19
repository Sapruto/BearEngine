#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "UIElement.h"
#include "Font.h"

class Text : public UIElement {
private:
    Font* font = nullptr;
    std::string text;
    UIRenderSettings settings;
    glm::vec4 color = glm::vec4(1.0f);

    void CalculateGeometry() override;

public:
    Text() = default;
    Text(Font& font, const std::string& text) 
        : font(&font), text(text) {
        settings.texture = const_cast<Texture*>(font.GetAtlas());
        settings.color = this->color;
    }
    
    const UIRenderSettings& GetRenderSettings() const override { return settings; }
    
    void SetText(const std::string& newText) { 
        text = newText; 
    }
    
    void SetFont(Font& newFont) {
        font = &newFont; 
        settings.texture = const_cast<Texture*>(newFont.GetAtlas());
    }

    void SetColor(glm::vec4 color) { settings.color = color; }

    glm::vec4 GetColor() const { return settings.color; }
    const std::string& GetText() const { return text; }
    Font* GetFont() const { return font; } 

    void SetFontSize(int size) { font->SetFontSize(size); }
};