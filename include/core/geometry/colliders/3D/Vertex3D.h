#pragma once

#include "Transform3D.h"
#include "Vector3.h"

struct Vertex3D {
    explicit Vertex3D(const Vector3& point) : localPoint(point) {}
    
    Vector3 localPoint;

    Vector3 GetGlobalPoint(const Transform3D& transform) const {
        return transform.GetGlobalRotation() * localPoint + transform.GetGlobalPosition();
    }
};