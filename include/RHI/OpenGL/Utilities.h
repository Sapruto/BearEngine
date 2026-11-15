#pragma once

#include "RHI/Base/Enums.h"
#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"

#include "glad/glad.h"
#include <string>

namespace RHI::OpenGL::Utilities {
    inline GLenum ToGL(CompareOp op) {
        switch (op) {
            case CompareOp::NEVER: return GL_NEVER;
            case CompareOp::LESS: return GL_LESS;
            case CompareOp::EQUAL: return GL_EQUAL;
            case CompareOp::LESS_EQUAL: return GL_LEQUAL;
            case CompareOp::GREATER: return GL_GREATER;
            case CompareOp::NOT_EQUAL: return GL_NOTEQUAL;
            case CompareOp::GREATER_EQUAL: return GL_GEQUAL;
            case CompareOp::ALWAYS: return GL_ALWAYS;
        }
        return GL_ALWAYS;
    }

    inline GLenum ToGL(StencilOp op) {
        switch (op) {
            case StencilOp::KEEP: return GL_KEEP;
            case StencilOp::ZERO: return GL_ZERO;
            case StencilOp::REPLACE: return GL_REPLACE;
            case StencilOp::INCREMENT_CLAMP: return GL_INCR;
            case StencilOp::DECREMENT_CLAMP: return GL_DECR;
            case StencilOp::INVERT: return GL_INVERT;
            case StencilOp::INCREMENT_WRAP: return GL_INCR_WRAP;
            case StencilOp::DECREMENT_WRAP: return GL_DECR_WRAP;
        }
        return GL_KEEP;
    }

    inline GLenum ToGL(BlendFactor f) {
        switch (f) {
            case BlendFactor::ZERO: return GL_ZERO;
            case BlendFactor::ONE: return GL_ONE;
            case BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
            case BlendFactor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
            case BlendFactor::DST_COLOR: return GL_DST_COLOR;
            case BlendFactor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
            case BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
            case BlendFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::DST_ALPHA: return GL_DST_ALPHA;
            case BlendFactor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
            case BlendFactor::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
            case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
            case BlendFactor::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
            case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
            case BlendFactor::SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
        }
        return GL_ONE;
    }

    inline GLenum ToGL(BlendOp op) {
        switch (op) {
            case BlendOp::ADD: return GL_FUNC_ADD;
            case BlendOp::SUBTRACT: return GL_FUNC_SUBTRACT;
            case BlendOp::REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
            case BlendOp::MIN: return GL_MIN;
            case BlendOp::MAX: return GL_MAX;
        }
        return GL_FUNC_ADD;
    }

    inline GLenum ToGL(PrimitiveTopology t) {
        switch (t) {
            case PrimitiveTopology::POINT_LIST: return GL_POINTS;
            case PrimitiveTopology::LINE_LIST: return GL_LINES;
            case PrimitiveTopology::LINE_STRIP: return GL_LINE_STRIP;
            case PrimitiveTopology::TRIANGLE_LIST: return GL_TRIANGLES;
            case PrimitiveTopology::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
            case PrimitiveTopology::TRIANGLE_FAN: return GL_TRIANGLE_FAN;
        }
        return GL_TRIANGLES;
    }

    inline GLenum ToGL(Format f) {
        switch (f) {
            case Format::UINT1: return GL_UNSIGNED_BYTE;
            case Format::UINT2: return GL_UNSIGNED_SHORT;
            case Format::UINT4: return GL_UNSIGNED_INT;
            case Format::DEPTH16: return GL_DEPTH_COMPONENT16;
            case Format::DEPTH24: return GL_DEPTH_COMPONENT24;
            case Format::DEPTH32F: return GL_DEPTH_COMPONENT32F;
            case Format::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
            case Format::DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;
            default: return GL_UNSIGNED_INT;
        }
    }

    inline GLint FormatComponents(Format f) {
        switch (f) {
            case Format::FLOAT1:
            case Format::HALF1:
            case Format::INT1:
            case Format::UINT1:
            case Format::SHORT1: case Format::SHORT1N:
            case Format::USHORT1: case Format::USHORT1N:
            case Format::BYTE1: case Format::BYTE1N:
            case Format::UBYTE1: case Format::UBYTE1N:
            case Format::DEPTH16:
            case Format::DEPTH24:
            case Format::DEPTH32F:
            case Format::DEPTH24_STENCIL8:
            case Format::DEPTH32F_STENCIL8:
                return 1;
            case Format::FLOAT2:
            case Format::HALF2:
            case Format::INT2:
            case Format::UINT2:
            case Format::SHORT2: case Format::SHORT2N:
            case Format::USHORT2: case Format::USHORT2N:
            case Format::BYTE2: case Format::BYTE2N:
            case Format::UBYTE2: case Format::UBYTE2N:
                return 2;
            case Format::FLOAT3:
            case Format::HALF3:
            case Format::INT3:
            case Format::UINT3:
            case Format::SHORT3: case Format::SHORT3N:
            case Format::USHORT3: case Format::USHORT3N:
            case Format::BYTE3: case Format::BYTE3N:
            case Format::UBYTE3: case Format::UBYTE3N:
                return 3;
            case Format::FLOAT4:
            case Format::HALF4:
            case Format::INT4:
            case Format::UINT4:
            case Format::SHORT4: case Format::SHORT4N:
            case Format::USHORT4: case Format::USHORT4N:
            case Format::BYTE4: case Format::BYTE4N:
            case Format::UBYTE4: case Format::UBYTE4N:
            case Format::UINT1010102N:
            case Format::INT1010102N:
            case Format::FLOAT_RG11B10:
            case Format::FLOAT_RGB9E5:
                return 4;
        }
        return 1;
    }

    inline GLenum FormatComponentType(Format f) {
        switch (f) {
            case Format::FLOAT1: case Format::FLOAT2:
            case Format::FLOAT3: case Format::FLOAT4:
                return GL_FLOAT;
            case Format::HALF1: case Format::HALF2:
            case Format::HALF3: case Format::HALF4:
                return GL_HALF_FLOAT;
            case Format::INT1: case Format::INT2:
            case Format::INT3: case Format::INT4:
            case Format::INT1010102N:
                return GL_INT;
            case Format::UINT1: case Format::UINT2:
            case Format::UINT3: case Format::UINT4:
            case Format::UINT1010102N:
                return GL_UNSIGNED_INT;
            case Format::SHORT1: case Format::SHORT2:
            case Format::SHORT3: case Format::SHORT4:
            case Format::SHORT1N: case Format::SHORT2N:
            case Format::SHORT3N: case Format::SHORT4N:
                return GL_SHORT;
            case Format::USHORT1: case Format::USHORT2:
            case Format::USHORT3: case Format::USHORT4:
            case Format::USHORT1N: case Format::USHORT2N:
            case Format::USHORT3N: case Format::USHORT4N:
                return GL_UNSIGNED_SHORT;
            case Format::BYTE1: case Format::BYTE2:
            case Format::BYTE3: case Format::BYTE4:
            case Format::BYTE1N: case Format::BYTE2N:
            case Format::BYTE3N: case Format::BYTE4N:
                return GL_BYTE;
            case Format::UBYTE1: case Format::UBYTE2:
            case Format::UBYTE3: case Format::UBYTE4:
            case Format::UBYTE1N: case Format::UBYTE2N:
            case Format::UBYTE3N: case Format::UBYTE4N:
                return GL_UNSIGNED_BYTE;
            case Format::FLOAT_RG11B10:
                return GL_UNSIGNED_INT_10F_11F_11F_REV;
            case Format::FLOAT_RGB9E5:
                return GL_UNSIGNED_INT_5_9_9_9_REV;

            case Format::DEPTH16: return GL_UNSIGNED_SHORT;
            case Format::DEPTH24: return GL_UNSIGNED_INT;
            case Format::DEPTH32F: return GL_FLOAT;
            case Format::DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
            case Format::DEPTH32F_STENCIL8: return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        }
        return GL_FLOAT;
    }

    inline GLboolean FormatIsNormalized(Format f) {
        switch (f) {
            case Format::SHORT1N: case Format::SHORT2N:
            case Format::SHORT3N: case Format::SHORT4N:
            case Format::USHORT1N: case Format::USHORT2N:
            case Format::USHORT3N: case Format::USHORT4N:
            case Format::BYTE1N: case Format::BYTE2N:
            case Format::BYTE3N: case Format::BYTE4N:
            case Format::UBYTE1N: case Format::UBYTE2N:
            case Format::UBYTE3N: case Format::UBYTE4N:
            case Format::UINT1010102N:
            case Format::INT1010102N:
                return GL_TRUE;
            default:
                return GL_FALSE;
        }
    }

    inline GLenum ToGLTarget(RHI::Executors::ResourceExecutor::BufferUsage u) {
        const uint32_t bits = static_cast<uint32_t>(u);
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::INDEX)) return GL_ELEMENT_ARRAY_BUFFER;
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::UNIFORM)) return GL_UNIFORM_BUFFER;
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::STORAGE)) return GL_SHADER_STORAGE_BUFFER;
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::INDIRECT)) return GL_DRAW_INDIRECT_BUFFER;
        return GL_ARRAY_BUFFER;
    }

    inline GLenum ToGLUsageHint(RHI::Executors::ResourceExecutor::BufferUsage u) {
        const uint32_t bits = static_cast<uint32_t>(u);
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::STAGING)) return GL_STREAM_DRAW;
        if (bits & static_cast<uint32_t>(RHI::Executors::ResourceExecutor::BufferUsage::COPY_DST)) return GL_DYNAMIC_DRAW;
        return GL_STATIC_DRAW;
    }

    inline GLenum ToGL(RHI::Executors::ResourceExecutor::FilterMode m) {
        switch (m) {
            case RHI::Executors::ResourceExecutor::FilterMode::NEAREST: return GL_NEAREST;
            case RHI::Executors::ResourceExecutor::FilterMode::LINEAR: return GL_LINEAR;
        }
        return GL_LINEAR;
    }

    inline GLenum ToGLMip(RHI::Executors::ResourceExecutor::FilterMode min, RHI::Executors::ResourceExecutor::MipFilterMode mip) {
        if (min == RHI::Executors::ResourceExecutor::FilterMode::NEAREST) {
            return mip == RHI::Executors::ResourceExecutor::MipFilterMode::NEAREST ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR;
        }
        return mip == RHI::Executors::ResourceExecutor::MipFilterMode::NEAREST ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    }

    inline GLenum ToGL(RHI::Executors::ResourceExecutor::AddressMode m) {
        switch (m) {
            case RHI::Executors::ResourceExecutor::AddressMode::REPEAT: return GL_REPEAT;
            case RHI::Executors::ResourceExecutor::AddressMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case RHI::Executors::ResourceExecutor::AddressMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
            case RHI::Executors::ResourceExecutor::AddressMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case RHI::Executors::ResourceExecutor::AddressMode::MIRROR_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
        }
        return GL_REPEAT;
    }

    inline GLenum FormatToGLInternalFormat(Format f) {
        switch (f) {
            case Format::FLOAT1: return GL_R32F;
            case Format::FLOAT2: return GL_RG32F;
            case Format::FLOAT3: return GL_RGB32F;
            case Format::FLOAT4: return GL_RGBA32F;

            case Format::HALF1: return GL_R16F;
            case Format::HALF2: return GL_RG16F;
            case Format::HALF3: return GL_RGB16F;
            case Format::HALF4: return GL_RGBA16F;

            case Format::INT1: return GL_R32I;
            case Format::INT2: return GL_RG32I;
            case Format::INT3: return GL_RGB32I;
            case Format::INT4: return GL_RGBA32I;

            case Format::UINT1: return GL_R32UI;
            case Format::UINT2: return GL_RG32UI;
            case Format::UINT3: return GL_RGB32UI;
            case Format::UINT4: return GL_RGBA32UI;

            case Format::SHORT1: return GL_R16I;
            case Format::SHORT2: return GL_RG16I;
            case Format::SHORT3: return GL_RGB16I;
            case Format::SHORT4: return GL_RGBA16I;
            case Format::SHORT1N: return GL_R16_SNORM;
            case Format::SHORT2N: return GL_RG16_SNORM;
            case Format::SHORT3N: return GL_RGB16_SNORM;
            case Format::SHORT4N: return GL_RGBA16_SNORM;

            case Format::USHORT1: return GL_R16UI;
            case Format::USHORT2: return GL_RG16UI;
            case Format::USHORT3: return GL_RGB16UI;
            case Format::USHORT4: return GL_RGBA16UI;
            case Format::USHORT1N: return GL_R16;
            case Format::USHORT2N: return GL_RG16;
            case Format::USHORT3N: return GL_RGB16;
            case Format::USHORT4N: return GL_RGBA16;

            case Format::BYTE1: return GL_R8I;
            case Format::BYTE2: return GL_RG8I;
            case Format::BYTE3: return GL_RGB8I;
            case Format::BYTE4: return GL_RGBA8I;
            case Format::BYTE1N: return GL_R8_SNORM;
            case Format::BYTE2N: return GL_RG8_SNORM;
            case Format::BYTE3N: return GL_RGB8_SNORM;
            case Format::BYTE4N: return GL_RGBA8_SNORM;

            case Format::UBYTE1: return GL_R8UI;
            case Format::UBYTE2: return GL_RG8UI;
            case Format::UBYTE3: return GL_RGB8UI;
            case Format::UBYTE4: return GL_RGBA8UI;
            case Format::UBYTE1N: return GL_R8;
            case Format::UBYTE2N: return GL_RG8;
            case Format::UBYTE3N: return GL_RGB8;
            case Format::UBYTE4N: return GL_RGBA8;

            case Format::UINT1010102N: return GL_RGB10_A2UI;
            case Format::INT1010102N: return GL_RGB10_A2;
            case Format::FLOAT_RG11B10: return GL_R11F_G11F_B10F;
            case Format::FLOAT_RGB9E5: return GL_RGB9_E5;

            case Format::DEPTH16: return GL_DEPTH_COMPONENT16;
            case Format::DEPTH24: return GL_DEPTH_COMPONENT24;
            case Format::DEPTH32F: return GL_DEPTH_COMPONENT32F;
            case Format::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
            case Format::DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;
        }
        return GL_RGBA8;
    }

    inline GLenum FormatToGLFormat(Format f) {
        switch (f) {
            case Format::FLOAT1:
            case Format::HALF1:
            case Format::INT1: case Format::UINT1:
            case Format::SHORT1: case Format::SHORT1N:
            case Format::USHORT1: case Format::USHORT1N:
            case Format::BYTE1: case Format::BYTE1N:
            case Format::UBYTE1: case Format::UBYTE1N:
                return GL_RED;

            case Format::FLOAT2:
            case Format::HALF2:
            case Format::INT2: case Format::UINT2:
            case Format::SHORT2: case Format::SHORT2N:
            case Format::USHORT2: case Format::USHORT2N:
            case Format::BYTE2: case Format::BYTE2N:
            case Format::UBYTE2: case Format::UBYTE2N:
                return GL_RG;

            case Format::FLOAT3:
            case Format::HALF3:
            case Format::INT3: case Format::UINT3:
            case Format::SHORT3: case Format::SHORT3N:
            case Format::USHORT3: case Format::USHORT3N:
            case Format::BYTE3: case Format::BYTE3N:
            case Format::UBYTE3: case Format::UBYTE3N:
                return GL_RGB;

            case Format::FLOAT4:
            case Format::HALF4:
            case Format::INT4: case Format::UINT4:
            case Format::SHORT4: case Format::SHORT4N:
            case Format::USHORT4: case Format::USHORT4N:
            case Format::BYTE4: case Format::BYTE4N:
            case Format::UBYTE4: case Format::UBYTE4N:
            case Format::UINT1010102N:
            case Format::INT1010102N:
            case Format::FLOAT_RG11B10:
            case Format::FLOAT_RGB9E5:
                return GL_RGBA;

            case Format::DEPTH16:
            case Format::DEPTH24:
            case Format::DEPTH32F:
                return GL_DEPTH_COMPONENT;

            case Format::DEPTH24_STENCIL8:
            case Format::DEPTH32F_STENCIL8:
                return GL_DEPTH_STENCIL;
        }
        return GL_RGBA;
    }

    inline GLenum FormatToGLType(Format f) {
        return FormatComponentType(f);
    }

    inline bool CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            errorMsg = std::string("Error ") + op + ": " + std::to_string(err);
            errorCode = err;
            return false;
        }
        return true;
    }

    inline unsigned int ToGLInterval(Executors::SwapChainExecutor::PresentMode mode) {
        switch (mode) {
            case Executors::SwapChainExecutor::PresentMode::FIFO: return 1;
            case Executors::SwapChainExecutor::PresentMode::IMMEDIATE: return 0;
            case Executors::SwapChainExecutor::PresentMode::FIFO_RELAXED: return 1;
            case Executors::SwapChainExecutor::PresentMode::MAILBOX: return 1;
        }
        return 1;
    }


    inline GLenum DetermineTextureTarget(const Executors::ResourceExecutor::TextureDesc& d) {
        using namespace Executors::ResourceExecutor;
        if (d.samples != SampleCount::COUNT_1) return GL_TEXTURE_2D_MULTISAMPLE;
        if (d.depth > 1) return GL_TEXTURE_3D;
        if (d.arrayLayers == 6 && d.width == d.height) return GL_TEXTURE_CUBE_MAP;
        if (d.arrayLayers > 1) return GL_TEXTURE_2D_ARRAY;
        return GL_TEXTURE_2D;
    }

    inline bool ValidateTextureDesc(const Executors::ResourceExecutor::TextureDesc& d, std::string& err) {
        using namespace Executors::ResourceExecutor;
        if (d.width == 0 || d.height == 0) { err = "Texture width/height must be > 0"; return false; }
        if (d.depth == 0) { err = "Texture depth must be > 0"; return false; }
        if (d.arrayLayers == 0) { err = "Texture arrayLayers must be > 0"; return false; }
        if (d.mipLevels == 0) { err = "Texture mipLevels must be > 0"; return false; }
        if (d.samples != SampleCount::COUNT_1 && d.mipLevels > 1) {
            err = "Multisample textures cannot have mip levels";
            return false;
        }
        if (d.samples != SampleCount::COUNT_1 && d.arrayLayers > 1) {
            err = "Multisample textures cannot be arrays";
            return false;
        }
        return true;
    }

    inline bool ValidateBufferDesc(const Executors::ResourceExecutor::BufferDesc& d, std::string& err) {
        if (d.size == 0) { err = "Buffer size must be > 0"; return false; }
        return true;
    }

    inline bool ValidateRenderTargetDesc(const Executors::ResourceExecutor::RenderTargetDesc& d, std::string& err) {
        if (d.width == 0 || d.height == 0) { err = "RenderTarget width/height must be > 0"; return false; }
        if (d.colorAttachments.empty() && d.depthStencilAttachment.id == 0) {
            err = "RenderTarget must have at least one attachment";
            return false;
        }
        return true;
    }

    inline bool ValidateMipLevel(uint32_t mip, uint32_t mipLevels, std::string& err) {
        if (mip >= mipLevels) { err = "mipLevel out of range"; return false; }
        return true;
    }

    inline bool ValidateArrayLayer(uint32_t layer, uint32_t layers, std::string& err) {
        if (layer >= layers) { err = "arrayLayer out of range"; return false; }
        return true;
    }
}