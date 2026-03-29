#pragma once

#include "UIRect.h"
#include "Texture.h"

#include <glm/glm.hpp>

struct UIRenderSettings{
    Texture* texture{nullptr};
    unsigned int textureID{0};
    
    UIRect uvRect = UIRect(0, 0, 1, 1); 
    
    glm::vec4 color{glm::vec4(1.0f)};
    
    int blendMode{0};         
    bool isMask{false}; 
    float alphaThreshold{0.1f}; 

    UIRenderSettings() = default;
    
    UIRenderSettings(Texture* tex) : texture(tex) {}
};