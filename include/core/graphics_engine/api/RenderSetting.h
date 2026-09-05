#pragma once

#include <string>
#include <vector>

struct RenderSetting {
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    int depthFunction = 2;
    
    bool cullFaceEnabled = true;
    int cullFaceMode = 0;
    
    bool blendEnabled = false;
    int blendSourceFactor = 1;
    int blendDestFactor = 0;
    
    bool wireframe = false;
    int polygonMode = 0;
    
    std::string shaderName;
    std::vector<std::string> textureNames;
    
    RenderSetting() = default;
    explicit RenderSetting(const std::string& shader) : shaderName(shader) {}

    bool operator==(const RenderSetting& other) const {
        return depthTestEnabled == other.depthTestEnabled &&
               depthWriteEnabled == other.depthWriteEnabled &&
               depthFunction == other.depthFunction &&
               cullFaceEnabled == other.cullFaceEnabled &&
               cullFaceMode == other.cullFaceMode &&
               blendEnabled == other.blendEnabled &&
               wireframe == other.wireframe &&
               shaderName == other.shaderName;
    }
    
    bool operator!=(const RenderSetting& other) const {
        return !(*this == other);
    }
};