#include "Tetrahedron3D.h"

#include <cmath>
#include <iostream>

float Tetrahedron3D::Determinant(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d) const
{
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 ad = d - a;
    
    return ab.dot(ac.cross(ad));
}

bool Tetrahedron3D::ContainsPoint(const Vector3& point, const Transform3D& transform) const {
    Vector3 p1 = vertices[0].GetGlobalPoint(transform);
    Vector3 p2 = vertices[1].GetGlobalPoint(transform);
    Vector3 p3 = vertices[2].GetGlobalPoint(transform);
    Vector3 p4 = vertices[3].GetGlobalPoint(transform);

    float d1 = Determinant(point, p2, p3, p4);
    float d2 = Determinant(p1, point, p3, p4);
    float d3 = Determinant(p1, p2, point, p4);
    float d4 = Determinant(p1, p2, p3, point);
    
    float epsilon = 0.0001f;
    
    return (d1 > -epsilon && d2 > -epsilon && d3 > -epsilon && d4 > -epsilon) ||
           (d1 < epsilon && d2 < epsilon && d3 < epsilon && d4 < epsilon);
}

float Tetrahedron3D::CalculateVolume(const Transform3D& transform) const {
    Vector3 ab = vertices[1].GetGlobalPoint(transform) - vertices[0].GetGlobalPoint(transform);
    Vector3 ac = vertices[2].GetGlobalPoint(transform) - vertices[0].GetGlobalPoint(transform);
    Vector3 ad = vertices[3].GetGlobalPoint(transform) - vertices[0].GetGlobalPoint(transform);
    
    float volume = std::abs(ad.dot(ab.cross(ac))) / 6.0f;
    
    return volume;
}