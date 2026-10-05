#pragma once

#include <variant>
#include <cstdint>
#include "RHI/RectRHI.h"

namespace RHI::Executors::DrawExecute {
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
        BaseDrawParams(uint32_t vao, uint32_t shaderProgram)
            : type(ParamType::BASE), vao(vao), shaderProgram(shaderProgram) {}
        ParamType GetType() const { return type; }
    };

    struct DrawElementsParam : public BaseDrawParams {
        struct Plain {
            int indexCount{0};
            uint32_t indexOffset{0};

            Plain() = default;
            Plain(int indexCount, uint32_t indexOffset)
                : indexCount(indexCount), indexOffset(indexOffset) {}
        };
        struct Range {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int minIndex{0};
            unsigned int maxIndex{0};

            Range() = default;
            Range(int indexCount, uint32_t indexOffset, unsigned int minIndex, unsigned int maxIndex)
                : indexCount(indexCount), indexOffset(indexOffset),
                  minIndex(minIndex), maxIndex(maxIndex) {}
        };
        struct Instanced {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int instanceCount{0};
            uint32_t baseInstance{0};

            Instanced() = default;
            Instanced(int indexCount, uint32_t indexOffset, unsigned int instanceCount, uint32_t baseInstance)
                : indexCount(indexCount), indexOffset(indexOffset),
                  instanceCount(instanceCount), baseInstance(baseInstance) {}
        };
        struct RangeInstanced {
            int indexCount{0};
            uint32_t indexOffset{0};
            unsigned int minIndex{0};
            unsigned int maxIndex{0};
            unsigned int instanceCount{0};
            uint32_t baseInstance{0};

            RangeInstanced() = default;
            RangeInstanced(int indexCount, uint32_t indexOffset, unsigned int minIndex,
                           unsigned int maxIndex, unsigned int instanceCount, uint32_t baseInstance)
                : indexCount(indexCount), indexOffset(indexOffset), minIndex(minIndex),
                  maxIndex(maxIndex), instanceCount(instanceCount), baseInstance(baseInstance) {}
        };

        DrawMode mode;
        IndexDataType indexDataType;
        unsigned int baseVertex = 0;

        std::variant<Plain, Range, Instanced, RangeInstanced> data{Plain{}};

        DrawElementsParam() { SetType(ParamType::DRAW_ELEMENTS); }

        DrawElementsParam(DrawMode mode, IndexDataType indexDataType, unsigned int baseVertex,
                          const std::variant<Plain, Range, Instanced, RangeInstanced>& data,
                          uint32_t vao = 0, uint32_t shaderProgram = 0)
            : BaseDrawParams(vao, shaderProgram), mode(mode), indexDataType(indexDataType),
              baseVertex(baseVertex), data(data) {
            SetType(ParamType::DRAW_ELEMENTS);
        }
    };

    struct DrawArrays : public BaseDrawParams {
        DrawMode mode;
        unsigned int first;
        unsigned int count;

        DrawArrays() { SetType(ParamType::DRAW_ARRAYS); }

        DrawArrays(DrawMode mode, unsigned int first, unsigned int count,
                   uint32_t vao = 0, uint32_t shaderProgram = 0)
            : BaseDrawParams(vao, shaderProgram), mode(mode), first(first), count(count) {
            SetType(ParamType::DRAW_ARRAYS);
        }
    };

    using DrawParams = std::variant<DrawElementsParam, DrawArrays>;
}