#pragma once

#include "include/core/math/Transform2D.h"
#include "include/core/math/Vector2.h"
#include "include/core/Component.h"

class Collider2D : public Component {
protected:
    Vector2 offset = Vector2(0.0f, 0.0f); 
    bool isTrigger = false;
    bool enabled = true;
    
public:
    Collider2D() = default;
    virtual ~Collider2D() = default;
    
    virtual Vector2 GetMinBounds() const = 0;
    virtual Vector2 GetMaxBounds() const = 0;
    virtual Vector2 GetCenter() const = 0;
    virtual Vector2 GetSize() const = 0;
    virtual void SetSize(const Vector2& newSize) = 0;
    virtual bool ContainsPoint(const Vector2& point) const = 0;
    virtual void GetWorldBounds(Vector2& outMin, Vector2& outMax) const = 0;
    virtual bool CheckCollision(Collider2D* other) const = 0;
    virtual bool Intersects(const Vector2& otherMin, const Vector2& otherMax) const = 0;
    
    Vector2 GetOffset() const { return offset; }
    bool IsTrigger() const { return isTrigger; }
    bool IsEnabled() const { return enabled; }
    
    void SetOffset(const Vector2& newOffset) { offset = newOffset; }
    void SetOffset(float x, float y) { offset = Vector2(x, y); }
    void SetTrigger(bool trigger) { isTrigger = trigger; }
    void SetEnabled(bool enable) { enabled = enable; }
};