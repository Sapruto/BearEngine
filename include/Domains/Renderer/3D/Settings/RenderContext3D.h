#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <vector>

#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector4.h"

#include "RHI/Base/Handles.h"
#include "../Settings/Lights/LightDataVariant.h"

namespace ResourceNames {
    constexpr const char* Albedo = "albedo";
    constexpr const char* Normal = "normal";
    constexpr const char* Depth = "depth";
    constexpr const char* MetallicRoughness = "metallicRoughness";
    constexpr const char* LitColor = "litColor";
    constexpr const char* Bloomed = "bloomed";
    constexpr const char* ShadowMap = "shadowMap";
    constexpr const char* SSAO = "ssao";
    constexpr const char* Velocity = "velocity";
    constexpr const char* TAAHistory = "taaHistory";
    constexpr const char* FinalColor = "finalColor";
}

struct RenderContext3D {
    Matrix4x4f view;
    Matrix4x4f projection;
    Vector3f viewPosition;
    Vector4f frustumPlanes[6];

    float aspect = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float time = 0.0f;
    uint32_t viewportWidth = 0;
    uint32_t viewportHeight = 0;

    std::unordered_map<std::string, RHI::Handles::TextureHandle> textures;
    std::unordered_map<std::string, RHI::Handles::BufferHandle> buffers;

    std::vector<LightData> lights;

    RHI::Handles::TextureHandle GetTexture(const std::string& name) const {
        auto it = textures.find(name);
        return it != textures.end() ? it->second : RHI::Handles::TextureHandle{};
    }
    void SetTexture(const std::string& name, RHI::Handles::TextureHandle h) {
        textures[name] = h;
    }

    RHI::Handles::BufferHandle GetBuffer(const std::string& name) const {
        auto it = buffers.find(name);
        return it != buffers.end() ? it->second : RHI::Handles::BufferHandle{};
    }
    void SetBuffer(const std::string& name, RHI::Handles::BufferHandle h) {
        buffers[name] = h;
    }

    void SetViewportSize(uint32_t width, uint32_t height) {
        viewportWidth = width;
        viewportHeight = height;
        aspect = (height != 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    }
};