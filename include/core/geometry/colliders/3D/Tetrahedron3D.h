#pragma once

#include "Vector3.h"
#include "Vertex3D.h"
#include <array>

struct Tetrahedron3D {
    std::array<Vertex3D, 4> vertices;

    Tetrahedron3D(const std::array<Vertex3D, 4>& verts) : vertices(verts) {}

    float Determinant(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d) const;
    
    bool ContainsPoint(const Vector3& point, const Transform3D& transform) const;
    
    float CalculateVolume(const Transform3D& transform) const;
};