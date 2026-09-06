#pragma once

#include <vector>
#include <string>
#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"

namespace GeometryExecute {
    struct DrawRectResult {
        float x, y;
        float width, height;
        bool isDrawn;
        size_t vertexCount;
        size_t indexCount;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct DrawCircleResult {
        float x, y;
        float radius;
        bool isDrawn;
        size_t vertexCount;
        size_t indexCount;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct DrawLineResult {
        float x1, y1;
        float x2, y2;
        bool isDrawn;
        size_t vertexCount;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct DrawTriangleResult {
        float x1, y1;
        float x2, y2;
        float x3, y3;
        bool isDrawn;
        size_t vertexCount;
        size_t indexCount;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct DrawTextResult {
        float x, y;
        std::string text;
        bool isDrawn;
        size_t characterCount;
        float textWidth;
        float textHeight;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct DrawTextureResult {
        float x, y;
        float width, height;
        unsigned int textureID;
        bool isDrawn;
        size_t vertexCount;
        size_t indexCount;
        float drawTimeMs;
        std::string errorMessage;
    };

    struct SetViewportResult {
        int x, y;
        int width, height;
        bool isSet;
        std::string errorMessage;
    };

    struct SetColorResult {
        Color color;
        bool isSet;
        std::string errorMessage;
    };

    struct SetBlendModeResult {
        BlendMode mode;
        bool isSet;
        std::string errorMessage;
    };

    struct ClearScreenResult {
        Color color;
        bool colorCleared;
        bool depthCleared;
        bool stencilCleared;
        float clearTimeMs;
        std::string errorMessage;
    };

    struct GeometryExecuteResult : public BaseProcessResult {
        enum class OperationType {
            DRAW_RECT,
            DRAW_CIRCLE,
            DRAW_LINE,
            DRAW_TRIANGLE,
            DRAW_TEXT,
            DRAW_TEXTURE,
            SET_VIEWPORT,
            SET_COLOR,
            SET_BLEND_MODE,
            CLEAR_SCREEN,
            UNKNOWN
        };

        OperationType operationType{OperationType::UNKNOWN};
        bool success{true};
        std::string errorMessage;
        float totalTimeMs{0.0f};
        
        std::variant<DrawRectResult,
                DrawCircleResult,
                DrawLineResult,
                DrawTriangleResult,
                DrawTextResult,
                DrawTextureResult,
                SetViewportResult,
                SetColorResult,
                SetBlendModeResult,
                ClearScreenResult> 
        data;
    };
}