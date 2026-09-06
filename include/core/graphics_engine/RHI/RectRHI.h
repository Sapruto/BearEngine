#pragma once

struct RectRHI {
    float x, y;
    float width, height;

    RectRHI() : x(0), y(0), width(0), height(0) {}
    explicit RectRHI(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}
};