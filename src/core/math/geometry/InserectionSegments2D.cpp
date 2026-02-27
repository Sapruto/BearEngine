#include "include/core/math/geometry/InserectionSegments2D.h"
#include <cmath>

int IntersectionSegments2D::sign(float value, float epsilon) {
    if (value > epsilon) return 1;
    if (value < -epsilon) return -1;
    return 0;
}

bool IntersectionSegments2D::isPointOnSegment(const Vector2& point, 
                                              const Vector2& segStart, 
                                              const Vector2& segEnd) {
    Vector2 segVec = segEnd - segStart;
    Vector2 pointVec = point - segStart;
    
    if (std::abs(Vector2::cross(segVec, pointVec)) > 1e-6f) {
        return false;
    }
    
    float dotProduct = pointVec.dot(segVec);
    float segLengthSq = segVec.sqrMagnitude();
    
    return dotProduct >= 0 && dotProduct <= segLengthSq;
}

bool IntersectionSegments2D::isPointOnSegment(const Vector2& point, 
                                              const Segment& segment) {
    return isPointOnSegment(point, segment.point1, segment.point2);
}

Vector2 IntersectionSegments2D::calculateIntersection(
    const Vector2& A, const Vector2& B,
    const Vector2& C, const Vector2& D) 
{
    Vector2 AB = B - A;
    Vector2 CD = D - C;
    Vector2 AC = C - A;
    
    float denominator = Vector2::cross(AB, CD);
    
    if (std::abs(denominator) < 1e-6f) {
        return Vector2(0, 0);  
    }
    
    float t = Vector2::cross(AC, CD) / denominator;
    return A + AB * t;
}

Vector2 IntersectionSegments2D::calculateIntersection(
    const Segment& seg1, const Segment& seg2) 
{
    return calculateIntersection(seg1.point1, seg1.point2, 
                                 seg2.point1, seg2.point2);
}

bool IntersectionSegments2D::checkIntersection(
    const Vector2& A, const Vector2& B,
    const Vector2& C, const Vector2& D, 
    Vector2* intersectionPoint) 
{
    Vector2 AB = B - A;
    
    Vector2 AC = C - A;
    Vector2 AD = D - A;
    
    float crossZ1 = Vector2::cross(AB, AC);
    float crossZ2 = Vector2::cross(AB, AD);
    
    int sign1 = sign(crossZ1);
    int sign2 = sign(crossZ2);
    
    bool firstCondition;
    
    if (sign1 == 0 || sign2 == 0) {
        if (sign1 == 0 && isPointOnSegment(C, A, B)) {
            if (intersectionPoint) *intersectionPoint = C;
            return true;
        }
        if (sign2 == 0 && isPointOnSegment(D, A, B)) {
            if (intersectionPoint) *intersectionPoint = D;
            return true;
        }
        firstCondition = true;
    } 
    else if (sign1 == sign2) {
        firstCondition = false;
    }
    else {
        firstCondition = true;
    }
    
    if (!firstCondition) {
        return false;
    }
    
    Vector2 CD = D - C;         
    Vector2 CA = A - C;      
    Vector2 CB = B - C;         
    
    float crossZ3 = Vector2::cross(CD, CA);  
    float crossZ4 = Vector2::cross(CD, CB);
    
    int sign3 = sign(crossZ3);
    int sign4 = sign(crossZ4);
    
    bool secondCondition;
    
    if (sign3 == 0 || sign4 == 0) {
        if (sign3 == 0 && isPointOnSegment(A, C, D)) {
            if (intersectionPoint) *intersectionPoint = A;
            return true;
        }
        if (sign4 == 0 && isPointOnSegment(B, C, D)) {
            if (intersectionPoint) *intersectionPoint = B;
            return true;
        }
        secondCondition = true;
    }
    else if (sign3 == sign4) {
        secondCondition = false;
    }
    else {
        secondCondition = true;
    }
    
    if (secondCondition) {
        if (intersectionPoint) {
            *intersectionPoint = calculateIntersection(A, B, C, D);
        }
    }
    
    return secondCondition;
}

bool IntersectionSegments2D::checkIntersection(
    const Segment& seg1, const Segment& seg2,
    Vector2* intersectionPoint) 
{
    return checkIntersection(seg1.point1, seg1.point2,
                             seg2.point1, seg2.point2,
                             intersectionPoint);
}

bool IntersectionSegments2D::checkIntersection(
    const Segment& segment,
    const Vector2& C, const Vector2& D,
    Vector2* intersectionPoint) 
{
    return checkIntersection(segment.point1, segment.point2,
                             C, D, intersectionPoint);
}

bool IntersectionSegments2D::checkIntersection(
    const Vector2& A, const Vector2& B,
    const Segment& segment,
    Vector2* intersectionPoint) 
{
    return checkIntersection(A, B,
                             segment.point1, segment.point2,
                             intersectionPoint);
}

bool IntersectionSegments2D::checkPointOnSegment(
    const Vector2& point, const Segment& segment) 
{
    return isPointOnSegment(point, segment.point1, segment.point2);
}

bool IntersectionSegments2D::getIntersectionPoint(
    const Segment& seg1, const Segment& seg2, Vector2& outPoint) 
{
    return checkIntersection(seg1, seg2, &outPoint);
}