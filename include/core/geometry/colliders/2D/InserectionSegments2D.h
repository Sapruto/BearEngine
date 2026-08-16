#pragma once

#include "Vector2.h"
#include <cmath>

struct Segment {
    Vector2f point1;
    Vector2f point2;
    
    Segment() : point1(), point2() {}
    Segment(const Vector2f& p1, const Vector2f& p2) : point1(p1), point2(p2) {}
    
    Vector2f getDirection() const { return point2 - point1; }
    float length() const { return point1.distanceTo(point2); }
    float sqrLength() const { return (point2 - point1).sqrMagnitude(); }
};

class IntersectionSegments2D {
private:
    static int sign(float value, float epsilon = 1e-6f);
    
    static bool isPointOnSegment(const Vector2f& point, 
                                 const Vector2f& segStart, 
                                 const Vector2f& segEnd);
    
    static bool isPointOnSegment(const Vector2f& point, 
                                 const Segment& segment);

    static Vector2f calculateIntersection(
        const Vector2f& A, const Vector2f& B,
        const Vector2f& C, const Vector2f& D);

    static Vector2f calculateIntersection(
        const Segment& seg1, const Segment& seg2);

public:
    static bool checkIntersection(
        const Vector2f& A, const Vector2f& B,
        const Vector2f& C, const Vector2f& D, 
        Vector2f* intersectionPoint = nullptr);

    static bool checkIntersection(
        const Segment& seg1, const Segment& seg2,
        Vector2f* intersectionPoint = nullptr);
    
    static bool checkIntersection(
        const Segment& segment,
        const Vector2f& C, const Vector2f& D,
        Vector2f* intersectionPoint = nullptr);
    
    static bool checkIntersection(
        const Vector2f& A, const Vector2f& B,
        const Segment& segment,
        Vector2f* intersectionPoint = nullptr);
    
    static bool checkPointOnSegment(
        const Vector2f& point, const Segment& segment);
    
    static bool intersects(const Segment& seg1, const Segment& seg2) {
        return checkIntersection(seg1, seg2, nullptr);
    }
    
    static bool getIntersectionPoint(
        const Segment& seg1, const Segment& seg2, Vector2f& outPoint);
};