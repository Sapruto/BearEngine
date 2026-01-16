#include "include/core/phisic_engine/BoxCollider2D.h"
#include "include/core/GameObject.h"
#include "include/core/phisic_engine/Collider2D.h"

#include "include/core/math/Transform2D.h"
#include "include/core/math/Vector2.h"

bool SegmentsIntersect(float aMin, float aMax, float bMin, float bMax) {
    if (aMax < bMin || bMax < aMin) {
        return false;
    }
    return true;
}


Vector2 BoxCollider2D::GetSize() const {
    return size;
}

void BoxCollider2D::SetSize(const Vector2& newSize) {
    size = newSize;
}

Vector2 BoxCollider2D::GetMinBounds() const {
    if (!gameObject) return Vector2::Zero();

    Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
    if (!transform) return offset;
    
    Vector2 worldPos = transform->position + offset;
    Vector2 halfSize = size * 0.5f;
    return worldPos - halfSize;
}

Vector2 BoxCollider2D::GetMaxBounds() const{
    if (!gameObject) return Vector2::Zero();

    Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
    if (!transform) return offset;
    
    Vector2 worldPos = transform->position + offset;
    Vector2 maxBounds = worldPos + (size * 0.5f);
    return maxBounds;
}

Vector2 BoxCollider2D::GetCenter() const{
    Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
    if (!transform) return offset;
    
    Vector2 worldPos = transform->position + offset;

    return worldPos;
}

bool BoxCollider2D::CheckCollision(Collider2D* other) const {
    if (!other || !gameObject) return false;

    Vector2 thisMin = GetMinBounds();
    Vector2 thisMax = GetMaxBounds();
    
    Vector2 otherMin = other->GetMinBounds();
    Vector2 otherMax = other->GetMaxBounds();
    
    bool intersectX = SegmentsIntersect(thisMin.x, thisMax.x, otherMin.x, otherMax.x);
    
    bool intersectY = SegmentsIntersect(thisMin.y, thisMax.y, otherMin.y, otherMax.y);
    
    return intersectX && intersectY;
}

bool BoxCollider2D::Intersects(const Vector2& otherMin, const Vector2& otherMax) const {
    Vector2 thisMin = GetMinBounds();
    Vector2 thisMax = GetMaxBounds();
    
    bool intersectX = SegmentsIntersect(thisMin.x, thisMax.x, otherMin.x, otherMax.x);
    
    bool intersectY = SegmentsIntersect(thisMin.y, thisMax.y, otherMin.y, otherMax.y);
    
    return intersectX && intersectY;
}
bool BoxCollider2D::ContainsPoint(const Vector2& point) const{
    Vector2 min = GetMinBounds();
    Vector2 max = GetMaxBounds();

    return (point.x >= min.x && point.x <= max.x) &&
           (point.y >= min.y && point.y <= max.y);
}
void BoxCollider2D::GetWorldBounds(Vector2& outMin, Vector2& outMax) const {
    Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
    if (!transform) {
        outMin = Vector2::Zero();
        outMax = Vector2::Zero();
        return;
    }
    
    Vector2 center = transform->position + offset;
    
    Vector2 halfSize = size * 0.5f;
    
    outMin = center - halfSize;
    outMax = center + halfSize;
}