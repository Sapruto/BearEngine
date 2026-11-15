#pragma once

#include "Math/Vector/Vector3.h"
#include "Math/Transform/Transform3D.h"
#include "Math/Geometry/Vertex.h"
#include <array>

struct Tetrahedron3D {
    std::array<Vertex3D, 4> vertices;

    Tetrahedron3D(const std::array<Vertex3D, 4>& verts) : vertices(verts) {}

    float Determinant(const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& d) const;
    
    bool ContainsPoint(const Vector3f& point, Transform3D* transform) const;
    
    float CalculateVolume(Transform3D* transform) const;
};