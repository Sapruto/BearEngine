#include "Font.h"

#include "Resource.h"
#include "Texture.h"
#include <string>
#include <glad/glad.h>

#include "Vector2.h"

FT_Library Font::library = nullptr;

bool Font::InitFreeType(){
    if (library) return true;
    
    FT_Error error = FT_Init_FreeType(&library);
    if (error){
        std::cerr << "Failed to initialize FreeType library. Error code: " << error << std::endl;
        return false;
    }
    
    return true;
}

bool Font::Load(){
    if(!library) return false;
    
    if(face) Unload();
    
    FT_Error error = FT_New_Face(library, GetPath().c_str(), 0, &face);
    if(error){
        std::cerr << "Failed to load font: " << GetPath() << std::endl;
        face = nullptr;
        return false;
    }
    
    error = FT_Set_Pixel_Sizes(face, 0, fontSize);
    if(error){
        std::cerr << "Failed to set font size" << std::endl;
        FT_Done_Face(face);
        face = nullptr;
        return false;
    }

    BuildAtlas();
    loaded = true;
    return true;
}

void Font::Unload(){
    if(atlas){
        atlas = nullptr;
    }
    if(face){
        FT_Done_Face(face);
        face = nullptr;
    }
    glyphs.clear();
}

bool Font::IsLoaded() const{
    return atlas != nullptr;
}

void Font::BuildAtlas(){
    if (!face) {
        std::cerr << "Font::BuildAtlas: face is null!" << std::endl;
        return;
    }

    int atlasWidth = 1024;
    int atlasHeight = 1024;
    int charX = 1;
    int charY = 1;
    int maxHeightInRow = 0;

    unsigned char* atlasData = new unsigned char[atlasWidth * atlasHeight * 4];
    memset(atlasData, 0, atlasWidth * atlasHeight * 4); 
    
    for (int i = 32; i <= 126; i++){
        FT_Load_Char(face, i, FT_LOAD_RENDER);
        FT_GlyphSlot slot = face->glyph;

        int glyphWidth = slot->bitmap.width;
        int glyphHeight = slot->bitmap.rows;
        
        if (glyphWidth == 0 || glyphHeight == 0) {
            GlyphData data;
            data.uvRect = UIRect(0, 0, 0, 0);
            data.advance = slot->advance.x >> 6;
            if (data.advance == 0) data.advance = fontSize / 2;
            data.bearing = Vector2(0, 0);
            data.size = Vector2(data.advance, fontSize);
            glyphs[static_cast<char>(i)] = data;
            
            continue;
        }
        
        if (charX + glyphWidth + 1 >= atlasWidth){
            charX = 1;
            charY += maxHeightInRow + 1;
            maxHeightInRow = 0;
        }
        
        for (int y = 0; y < glyphHeight; y++){
            for (int x = 0; x < glyphWidth; x++){
                unsigned char intensity = slot->bitmap.buffer[y * glyphWidth + x];
                
                int atlasX = charX + x;
                int atlasY = charY + y;
                int index = (atlasY * atlasWidth + atlasX) * 4;
                
                atlasData[index + 0] = intensity;
                atlasData[index + 1] = intensity; 
                atlasData[index + 2] = intensity;  
                atlasData[index + 3] = intensity; 
            }
        }
        
        GlyphData data;
        data.uvRect = UIRect(
            (float)(charX) / atlasWidth,
            (float)(charY) / atlasHeight,
            (float)glyphWidth / atlasWidth,
            (float)glyphHeight / atlasHeight
        );
        data.advance = slot->advance.x >> 6;
        data.bearing = Vector2(slot->bitmap_left, slot->bitmap_top);
        data.size = Vector2(glyphWidth, glyphHeight);
        
        glyphs[static_cast<char>(i)] = data;

        charX += glyphWidth + 1;
        maxHeightInRow = std::max(maxHeightInRow, glyphHeight);
    }
    
    atlas = Texture::CreateFromData(atlasData, atlasWidth, atlasHeight, GL_RGBA);
    
    delete[] atlasData;
}

const GlyphData& Font::GetGlyphData(char glyph) const {
    auto it = glyphs.find(glyph);
    if (it == glyphs.end()) {
        static GlyphData empty;
        return empty;
    }
    return it->second;
}