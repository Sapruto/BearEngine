#pragma once

#include <cstdint>

namespace RHI {
    enum class ShaderStage {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE,
        TESS_CONTROL,
        TESS_EVAL
    };

    enum class PrimitiveTopology {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };

    enum class FillMode {
        SOLID,
        WIREFRAME,
        POINT
    };

    enum class CullMode {
        NONE,
        FRONT,
        BACK,
        FRONT_AND_BACK
    };

    enum class FrontFace {
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class CompareOp {
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS
    };

    enum class StencilOp {
        KEEP,
        ZERO,
        REPLACE,
        INCREMENT_CLAMP,
        DECREMENT_CLAMP,
        INVERT,
        INCREMENT_WRAP,
        DECREMENT_WRAP
    };

    enum class BlendFactor {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
        SRC_ALPHA_SATURATE
    };

    enum class BlendOp {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    enum class ColorWriteMask : uint8_t {
        NONE = 0,
        R = 1u << 0,
        G = 1u << 1,
        B = 1u << 2,
        A = 1u << 3,
        ALL = R | G | B | A
    };

    constexpr ColorWriteMask operator&(ColorWriteMask a, ColorWriteMask b) noexcept {
        return static_cast<ColorWriteMask>(
            static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    constexpr ColorWriteMask operator|(ColorWriteMask a, ColorWriteMask b) noexcept {
        return static_cast<ColorWriteMask>(
            static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    constexpr ColorWriteMask operator^(ColorWriteMask a, ColorWriteMask b) noexcept {
        return static_cast<ColorWriteMask>(
            static_cast<uint8_t>(a) ^ static_cast<uint8_t>(b));
    }

    constexpr ColorWriteMask operator~(ColorWriteMask a) noexcept {
        return static_cast<ColorWriteMask>(
            static_cast<uint8_t>(~static_cast<uint8_t>(a)));
    }

    enum class SampleCount {
        COUNT_1 = 1,
        COUNT_2 = 2,
        COUNT_4 = 4,
        COUNT_8 = 8,
        COUNT_16 = 16
    };

    enum class DynamicState {
        VIEWPORT,
        SCISSOR,
        LINE_WIDTH,
        DEPTH_BIAS,
        BLEND_CONSTANTS,
        DEPTH_BOUNDS,
        STENCIL_COMPARE_MASK,
        STENCIL_WRITE_MASK,
        STENCIL_REFERENCE
    };

    enum class Format : uint32_t {
        FLOAT1, FLOAT2, FLOAT3, FLOAT4,

        HALF1, HALF2, HALF3, HALF4,

        INT1, INT2, INT3, INT4,

        UINT1, UINT2, UINT3, UINT4,

        SHORT1, SHORT2, SHORT3, SHORT4,
        SHORT1N, SHORT2N, SHORT3N, SHORT4N,

        USHORT1, USHORT2, USHORT3, USHORT4,
        USHORT1N, USHORT2N, USHORT3N, USHORT4N,

        BYTE1, BYTE2, BYTE3, BYTE4,
        BYTE1N, BYTE2N, BYTE3N, BYTE4N,

        UBYTE1, UBYTE2, UBYTE3, UBYTE4,
        UBYTE1N, UBYTE2N, UBYTE3N, UBYTE4N,

        UINT1010102N,
        INT1010102N,
        FLOAT_RG11B10,
        FLOAT_RGB9E5,

        DEPTH16,
        DEPTH24,
        DEPTH32F,
        DEPTH24_STENCIL8,
        DEPTH32F_STENCIL8,
    };
};