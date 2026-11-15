#pragma once

#include "RHI/Base/Handles.h"

#include <string>
#include <optional>

namespace Renderer::Base {
    struct ShaderPathes {
        RHI::Handles::ShaderHandle vertexPath;
        RHI::Handles::ShaderHandle fragmePath;
        std::optional<RHI::Handles::ShaderHandle> geometryPath;
        std::optional<RHI::Handles::ShaderHandle> computePath;
        std::optional<RHI::Handles::ShaderHandle> tessControlPath;
        std::optional<RHI::Handles::ShaderHandle> tessEvalPath;
    };

    struct ShaderState {
        std::string vertexHandle;
        std::string fragmeHanle;
        std::optional<std::string> geometryHandle;
        std::optional<std::string> computeHandle;
        std::optional<std::string> tessControlHandle;
        std::optional<std::string> tessEvalHandle;
    };
    
    class Shader {
    private:
        ShaderState state;
        
    public:
        Shader() = default;
        Shader(const ShaderPathes& pathes);

        bool Initialize(const ShaderPathes& pathes);
        void Shutdown();

        const ShaderState& GetShaderState();
    };
}