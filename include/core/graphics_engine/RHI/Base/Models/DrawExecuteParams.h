#pragma once

#include <variant>
#include <cstdint>
#include "RHI/RectRHI.h"

namespace DrawExecute {
    enum class ParamType : uint8_t {
        BASE,
        DRAW_ELEMENTS,
        DRAW_ARRAYS,
        COUNT
    };

    enum class DrawMode {
        POINTS,
        
        LINES,
        LINE_STRIP,

        TRIANGLES,
        TRIANGLE_STRIP
    };

    enum class IndexDataType {
        UINT8,
        UINT16,
        UINT32
    };

    struct BaseDrawParams {
    private:
        ParamType type;

    protected:
        void SetType(ParamType newType) { type = newType; }

    public:
        uint32_t vao = 0;
        uint32_t shaderProgram = 0;

        BaseDrawParams() : type(ParamType::BASE) {}
        ParamType GetType() const { return type; }
    };

    struct DrawElementsParam : public BaseDrawParams {
        struct Plain {
            int indexCount{0};
            uint32_t indexOffset{0};
        };
        struct Range {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int minIndex{0};
            unsigned int maxIndex{0};
        };
        struct Instanced {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int instanceCount{0};
            uint32_t baseInstance{0};
        };
        struct RangeInstanced {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int minIndex{0};
            unsigned int maxIndex{0};
            unsigned int instanceCount{0};
            uint32_t baseInstance{0};
        };

        DrawMode mode;
        IndexDataType indexDataType;;
        unsigned int baseVertex = 0;

        std::variant<Plain, Range, Instanced, RangeInstanced> data{Plain{}};

        DrawElementsParam() { SetType(ParamType::DRAW_ELEMENTS); }
    };

    struct DrawArrays : public BaseDrawParams {
        DrawMode mode;
        unsigned int first;
        unsigned int count;

        DrawArrays() { SetType(ParamType::DRAW_ARRAYS); }
    };

    using DrawParams = std::variant<DrawElementsParam, DrawArrays>;
};