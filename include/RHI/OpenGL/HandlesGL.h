#pragma once

#include "RHI/Base/Enums.h"
#include "RHI/Base/Handles.h"
#include "RHI/Base/Contracts/PipelineExecutorParams.h"

#include "glad/glad.h"
#include <variant>
#include <vector>

namespace RHI::OpenGL {
    struct BufferGL {
        GLuint id = 0;
        GLenum target = GL_ARRAY_BUFFER;
        void Destroy() { if (id) { glDeleteBuffers(1, &id); id = 0; } }
    };

    struct TextureGL {
        GLuint id = 0;
        GLenum target = GL_TEXTURE_2D;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t mipLevels = 1;
        uint32_t arrayLayers = 1;
        RHI::Format format = RHI::Format::FLOAT4;
        RHI::SampleCount samples = RHI::SampleCount::COUNT_1;

        bool isArray = false;
        bool is3D = false;
        bool isCube = false;
        bool isMultisample = false;

        void Destroy() { if (id) { glDeleteTextures(1, &id); id = 0; } }
    };

    struct SamplerGL {
        GLuint id = 0;
        void Destroy() { if (id) { glDeleteSamplers(1, &id); id = 0; } }
    };

    struct ShaderGL {
        GLuint id = 0;
        void Destroy() { if (id) { glDeleteShader(id); id = 0; } }
    };

    struct PipelineGL {
        GLuint program = 0;
        GLuint vao = 0;
        RHI::Executors::PipelineExecutor::PipelineState state;
        void Destroy() {
            if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
            if (program) { glDeleteProgram(program); program = 0; }
        }
    };

    struct RenderTargetGL {
        GLuint fbo = 0;
        std::vector<RHI::Handles::TextureHandle> colors;
        std::vector<RHI::Handles::TextureHandle> resolveTargets;
        RHI::Handles::TextureHandle depth;
        RHI::SampleCount samples = RHI::SampleCount::COUNT_1;
        uint32_t width = 0;
        uint32_t height = 0;

        void Destroy() { if (fbo) { glDeleteFramebuffers(1, &fbo); fbo = 0; } }
    };

    struct NativeHandleGL : std::variant<BufferGL, TextureGL, SamplerGL, ShaderGL, PipelineGL, RenderTargetGL> {
        using variant::variant;
        void Destroy() {
            std::visit([](auto& n) { n.Destroy(); }, *this);
        }
    };
}