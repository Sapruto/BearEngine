#pragma once

#include <variant>
#include "RHI/RHI/RectRHI.h"

namespace GeometryExecute {
    enum class BlendMode : uint8_t {
        Normal,
        Add,
        Multiply,
        Screen,
        
        AlphaPremultiplied,
        Additive,
        Subtractive,
        Darken,
        Lighten,
        
        ColorBurn,
        ColorDodge,
        Overlay,
        SoftLight,
        HardLight,
        
        Mask,
        Stencil
    };

    struct Color {
        float r;
        float g;
        float b;
        float a = 1.0f;
    };

    struct DrawRectParams {
        RectRHI rect;
        Color color;
        float rotation = 0.0f;
        bool filled = true;
        bool outline = false;
        float outlineThickness = 1.0f;
    };

    struct DrawCircleParams {
        float x, y;
        float radius;
        Color color;
        float rotation = 0.0f;
        bool filled = true;
        bool outline = false;
        float outlineThickness = 1.0f;
    };

    struct DrawLineParams {
        float x1, y1;
        float x2, y2;
        Color color;
        float thickness = 1.0f;
    };

    struct DrawTriangleParams {
        float x1, y1;
        float x2, y2;
        float x3, y3;
        Color color;
        bool filled = true;
        bool outline = false;
        float outlineThickness = 1.0f;
    };

    struct DrawTextParams {
        float x, y;
        std::string text;
        std::string fontName;
        int fontSize;
        Color color;
        float rotation = 0.0f;
    };

    struct DrawTextureParams {
        RectRHI rect;
        unsigned int textureID;
        Color colorMod = Color(1.0f, 1.0f, 1.0f);
        float rotation = 0.0f;
        float uvX = 0.0f, uvY = 0.0f;
        float uvWidth = 1.0f, uvHeight = 1.0f;
    };

    struct SetViewportParams {
        RectRHI rect;
        float minDepth{0.0f};
        float maxDepth{1.0f};
    };

    struct SetColorParams {
        Color color;
    };

    struct SetBlendModeParams {
        BlendMode mode;
    };

    struct ClearScreenParams {
        Color color;
        bool clearColor = true;
        bool clearDepth = true;
        bool clearStencil = false;
    };

    using GeometryParams = std::variant<
        DrawRectParams,
        DrawCircleParams,
        DrawLineParams,
        DrawTriangleParams,
        DrawTextParams,
        DrawTextureParams,
        SetViewportParams,
        SetColorParams,
        SetBlendModeParams,
        ClearScreenParams
    >;
};