#pragma once

#include <string_view>

enum class ResourceType {
    Texture,
    Font,
    Audio,
    Mesh
};

namespace ResourceTypeNames {
    inline constexpr std::string_view Texture = "texture";
    inline constexpr std::string_view Font = "font";
    inline constexpr std::string_view Audio = "audio";
    inline constexpr std::string_view Mesh = "mesh";
}

namespace ResourceTypeUtils {
    inline constexpr std::string_view ToString(ResourceType type) {
        switch (type) {
            case ResourceType::Texture: return ResourceTypeNames::Texture;
            case ResourceType::Font: return ResourceTypeNames::Font;
            case ResourceType::Audio: return ResourceTypeNames::Audio;
            case ResourceType::Mesh: return ResourceTypeNames::Mesh;
            default: return "unknown";
        }
    }

    inline constexpr ResourceType FromString(std::string_view str) {
        if (str == ResourceTypeNames::Texture) return ResourceType::Texture;
        if (str == ResourceTypeNames::Font) return ResourceType::Font;
        if (str == ResourceTypeNames::Audio) return ResourceType::Audio;
        if (str == ResourceTypeNames::Mesh) return ResourceType::Mesh;
        return ResourceType::Texture;
    }
};