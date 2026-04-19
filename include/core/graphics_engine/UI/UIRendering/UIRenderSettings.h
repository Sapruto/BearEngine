#pragma once

#include "UIRect.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <string>
#include <sstream>

struct UIRenderSettings {
    Texture* texture = nullptr;
    unsigned int textureID = 0;
    UIRect uvRect = UIRect(0, 0, 1, 1);
    glm::vec4 color = glm::vec4(1.0f);
    int blendMode = 0;
    bool isMask = false;
    float alphaThreshold = 0.1f;

    UIRenderSettings() = default;
    UIRenderSettings(Texture* tex) : texture(tex) {}
    
    std::string ToString() const {
        std::stringstream ss;
        ss << textureID << "|"
           << uvRect.x << "," << uvRect.y << "," << uvRect.width << "," << uvRect.height << "|"
           << color.r << "," << color.g << "," << color.b << "," << color.a << "|"
           << blendMode << "|"
           << isMask << "|"
           << alphaThreshold;
        return ss.str();
    }
    
    static UIRenderSettings FromString(const std::string& str) {
        UIRenderSettings settings;
        std::stringstream ss(str);
        std::string token;
        
        std::getline(ss, token, '|');
        settings.textureID = std::stoi(token);
        
        std::getline(ss, token, '|');
        sscanf(token.c_str(), "%f,%f,%f,%f", &settings.uvRect.x, &settings.uvRect.y, &settings.uvRect.width, &settings.uvRect.height);
        
        std::getline(ss, token, '|');
        sscanf(token.c_str(), "%f,%f,%f,%f", &settings.color.r, &settings.color.g, &settings.color.b, &settings.color.a);
        
        std::getline(ss, token, '|');
        settings.blendMode = std::stoi(token);
        
        std::getline(ss, token, '|');
        settings.isMask = (token == "1" || token == "true");
        
        std::getline(ss, token);
        settings.alphaThreshold = std::stof(token);
        
        return settings;
    }
};