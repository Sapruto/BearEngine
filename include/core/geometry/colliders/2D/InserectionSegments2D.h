#pragma once

#include "Vector2.h"
#include <cmath>

struct Segment {
    Vector2 point1;
    Vector2 point2;
    
    Segment() : point1(), point2() {}
    Segment(const Vector2& p1, const Vector2& p2) : point1(p1), point2(p2) {}
    
    Vector2 getDirection() const { return point2 - point1; }
    float length() const { return point1.distanceTo(point2); }
    float sqrLength() const { return (point2 - point1).sqrMagnitude(); }
};

class IntersectionSegments2D {
private:
    static int sign(float value, float epsilon = 1e-6f);
    
    static bool isPointOnSegment(const Vector2& point, 
                                 const Vector2& segStart, 
                                 const Vector2& segEnd);
    
    static bool isPointOnSegment(const Vector2& point, 
                                 const Segment& segment);

    static Vector2 calculateIntersection(
        const Vector2& A, const Vector2& B,
        const Vector2& C, const Vector2& D);

    static Vector2 calculateIntersection(
        const Segment& seg1, const Segment& seg2);

public:
    static bool checkIntersection(
        const Vector2& A, const Vector2& B,
        const Vector2& C, const Vector2& D, 
        Vector2* intersectionPoint = nullptr);

    static bool checkIntersection(
        const Segment& seg1, const Segment& seg2,
        Vector2* intersectionPoint = nullptr);
    
    static bool checkIntersection(
        const Segment& segment,
        const Vector2& C, const Vector2& D,
        Vector2* intersectionPoint = nullptr);
    
    static bool checkIntersection(
        const Vector2& A, const Vector2& B,
        const Segment& segment,
        Vector2* intersectionPoint = nullptr);
    
    static bool checkPointOnSegment(
        const Vector2& point, const Segment& segment);
    
    static bool intersects(const Segment& seg1, const Segment& seg2) {
        return checkIntersection(seg1, seg2, nullptr);
    }
    
    static bool getIntersectionPoint(
        const Segment& seg1, const Segment& seg2, Vector2& outPoint);
};