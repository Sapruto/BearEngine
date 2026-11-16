#pragma once

#include <filesystem>

namespace ModelsShaderPaths {
    inline const std::filesystem::path Base = "Shaders";

    namespace Default {
        inline const std::filesystem::path Vertex = (Base / "Default" / "Vertex.glsl");
        inline const std::filesystem::path Fragment = (Base / "Default" / "Fragment.glsl");
    }
}