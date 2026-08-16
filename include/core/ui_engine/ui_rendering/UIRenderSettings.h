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
    float color[4];
    int blendMode = 0;
    bool isMask = false;
    float alphaThreshold = 0.1f;

    UIRenderSettings() : color{0, 0, 0, 0} {}
    UIRenderSettings(Texture* tex) : texture(tex), color{0, 0, 0, 0} {}
    
    static std::string ToString(const UIRenderSettings& set) {
        std::stringstream ss;
        ss << set.textureID << "|"
        << set.uvRect.x << "," << set.uvRect.y << "," << set.uvRect.width << "," << set.uvRect.height << "|"
        << set.color[0] << "," << set.color[1] << "," << set.color[2] << "," << set.color[3] << "|"
        << set.blendMode << "|"
        << set.isMask << "|"
        << set.alphaThreshold;
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
        sscanf(token.c_str(), "%f,%f,%f,%f", &settings.color[0], &settings.color[1], &settings.color[2], &settings.color[3]);
        
        std::getline(ss, token, '|');
        settings.blendMode = std::stoi(token);
        
        std::getline(ss, token, '|');
        settings.isMask = (token == "1" || token == "true");
        
        std::getline(ss, token);
        settings.alphaThreshold = std::stof(token);
        
        return settings;
    }
};