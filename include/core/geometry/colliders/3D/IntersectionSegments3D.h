#pragma once

#include "Vector3.h"

struct Segment3D {
    Vector3 point1;
    Vector3 point2;
    
    Segment3D() : point1(), point2() {}
    Segment3D(const Vector3& p1, const Vector3& p2) : point1(p1), point2(p2) {}
    
    Vector3 getDirection() const { return point2 - point1; }
    float length() const { return point1.distanceTo(point2); }
    float sqrLength() const { return (point2 - point1).sqrMagnitude(); }
};

class IntersectionSegments3D {
private:
    static int Sign(float value, float epsilon = 1e-6f);
    static bool IsPointOnSegment(const Vector3& point, const Vector3& segStart, const Vector3& segEnd);
    static Vector3 CalculateIntersectionPoint(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D);

public:
    static bool CheckIntersection(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D, Vector3* intersectionPoint = nullptr);
    static bool CheckIntersection(const Segment3D& seg1, const Segment3D& seg2, Vector3* intersectionPoint = nullptr);
    static bool CheckIntersection(const Segment3D& segment, const Vector3& C, const Vector3& D, Vector3* intersectionPoint = nullptr);
    static bool CheckIntersection(const Vector3& A, const Vector3& B, const Segment3D& segment, Vector3* intersectionPoint = nullptr);
    
    static bool IsPointOnSegment(const Vector3& point, const Segment3D& segment);
    static bool Intersects(const Segment3D& seg1, const Segment3D& seg2);
    static bool GetIntersectionPoint(const Segment3D& seg1, const Segment3D& seg2, Vector3& outPoint);
};