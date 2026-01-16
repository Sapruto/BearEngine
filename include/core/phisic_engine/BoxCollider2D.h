#pragma once

#include "include/core/math/Transform2D.h"
#include "include/core/math/Vector2.h"
#include "include/core/Component.h"
#include "include/core/phisic_engine/Collider2D.h"

class BoxCollider2D : public Collider2D {
private:
    Vector2 size = Vector2(1.0f, 1.0f);
    
public:
    BoxCollider2D() = default;
    ~BoxCollider2D() = default;
    
    Vector2 GetSize() const override;
    void SetSize(const Vector2& newSize) override;
    Vector2 GetMinBounds() const override;
    Vector2 GetMaxBounds() const override;
    Vector2 GetCenter() const override;
    bool ContainsPoint(const Vector2& point) const override;
    void GetWorldBounds(Vector2& outMin, Vector2& outMax) const override;
    bool CheckCollision(Collider2D* other) const override;
    bool Intersects(const Vector2& otherMin, const Vector2& otherMax) const override;
    
    void SetSize(float width, float height) { size = Vector2(width, height); }
};