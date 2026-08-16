#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Vector2.h"
#include "Resource.h"
#include <string>
#include <unordered_map> 

#include "UIRect.h"

class Texture;

struct GlyphData {
    UIRect uvRect;   
    
    float advance;      
    Vector2f bearing;    
    Vector2f size;   
};

class Font : public Resource {
private:
    static FT_Library library;

    FT_Face face = nullptr;

    std::unordered_map<char, GlyphData> glyphs;

    Texture* atlas{nullptr};    

    int fontSize = 48;        
    bool loaded = false;

    void BuildAtlas();

public:
    explicit Font(const std::string& path) : Resource(path) {}
    ~Font() override;

    static bool InitFreeType();
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override;

    const GlyphData& GetGlyphData(char glyph) const;

    Texture* GetAtlas() { return atlas; } 
    const Texture* GetAtlas() const { return atlas; }

    int GetSize() { return fontSize; }
    int GetSize() const { return fontSize; }

    void SetFontSize(int fontSize) { this->fontSize = fontSize; }
};