#pragma once

#include "Vector2.h"
#include "Transform.h"

class Transform2D : public Transform {
public:
    Vector2 position;
    Vector2 scale;
    float rotation;
    
    Transform2D() : position(0, 0), scale(1, 1), rotation(0) {}
    
    void rotate(float angleRad);
    
    Vector2 forward() const;
    Vector2 right() const;
    
    void lookAt(const Vector2& target);
};