#pragma once

struct UIRect {
    float x, y, width, height;
    
    UIRect(float x = 0, float y = 0, float w = 0, float h = 0) 
        : x(x), y(y), width(w), height(h) {}
};