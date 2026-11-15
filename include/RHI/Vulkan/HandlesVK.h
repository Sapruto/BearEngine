#pragma once

#include "RHI/Base/Enums.h"
#include "RHI/Base/Handles.h"
#include "RHI/Base/Contracts/PipelineExecutorParams.h"

#include "glad/glad.h"
#include <variant>
#include <vector>

namespace RHI::Vulkan {
    struct BufferVK {
        GLuint id = 0;
        GLenum target = GL_ARRAY_BUFFER;
        void Destroy() { if (id) { glDeleteBuffers(1, &id); id = 0; } }
    };

    struct TextureVK {
        GLuint id = 0;
        GLenum target = GL_TEXTURE_2D;
        uint32_t width = 1;
        uint32_t height = 1;
        RHI::Format format = RHI::Format::FLOAT4;
        void Destroy() { if (id) { glDeleteTextures(1, &id); id = 0; } }
    };

    struct SamplerVK {
        GLuint id = 0;
        void Destroy() { if (id) { glDeleteSamplers(1, &id); id = 0; } }
    };

    struct ShaderVK {
        GLuint id = 0;
        void Destroy() { if (id) { glDeleteShader(id); id = 0; } }
    };

    struct PipelineVK {
        GLuint program = 0;
        GLuint vao = 0;
        RHI::Executors::PipelineExecutor::PipelineState state;
        void Destroy() {
            if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
            if (program) { glDeleteProgram(program); program = 0; }
        }
    };

    struct RenderTargetVK {
        GLuint fbo = 0;
        std::vector<RHI::Handles::TextureHandle> colors;
        RHI::Handles::TextureHandle depth;
        void Destroy() {  }
    };

    struct NativeHandleVK : std::variant<BufferVK, TextureVK, SamplerVK, ShaderVK, PipelineVK, RenderTargetVK> {
        using variant::variant;
        void Destroy() {
            std::visit([](auto& n) { n.Destroy(); }, *this);
        }
    };
}