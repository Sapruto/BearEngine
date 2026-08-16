#include "InserectionSegments2D.h"
#include <cmath>

int IntersectionSegments2D::sign(float value, float epsilon) {
    if (value > epsilon) return 1;
    if (value < -epsilon) return -1;
    return 0;
}

bool IntersectionSegments2D::isPointOnSegment(const Vector2f& point, 
                                              const Vector2f& segStart, 
                                              const Vector2f& segEnd) {
    Vector2f segVec = segEnd - segStart;
    Vector2f pointVec = point - segStart;
    
    if (std::abs(Vector2f::Cross(segVec, pointVec)) > 1e-6f) {
        return false;
    }
    
    float dotProduct = pointVec.dot(segVec);
    float segLengthSq = segVec.sqrMagnitude();
    
    return dotProduct >= 0 && dotProduct <= segLengthSq;
}

bool IntersectionSegments2D::isPointOnSegment(const Vector2f& point, 
                                              const Segment& segment) {
    return isPointOnSegment(point, segment.point1, segment.point2);
}

Vector2f IntersectionSegments2D::calculateIntersection(
    const Vector2f& A, const Vector2f& B,
    const Vector2f& C, const Vector2f& D) 
{
    Vector2f AB = B - A;
    Vector2f CD = D - C;
    Vector2f AC = C - A;
    
    float denominator = Vector2f::Cross(AB, CD);
    
    if (std::abs(denominator) < 1e-6f) {
        return Vector2f(0, 0);  
    }
    
    float t = Vector2f::Cross(AC, CD) / denominator;
    return A + AB * t;
}

Vector2f IntersectionSegments2D::calculateIntersection(
    const Segment& seg1, const Segment& seg2) 
{
    return calculateIntersection(seg1.point1, seg1.point2, 
                                 seg2.point1, seg2.point2);
}

bool IntersectionSegments2D::checkIntersection(
    const Vector2f& A, const Vector2f& B,
    const Vector2f& C, const Vector2f& D, 
    Vector2f* intersectionPoint) 
{
    Vector2f AB = B - A;
    
    Vector2f AC = C - A;
    Vector2f AD = D - A;
    
    float CrossZ1 = Vector2f::Cross(AB, AC);
    float CrossZ2 = Vector2f::Cross(AB, AD);
    
    int sign1 = sign(CrossZ1);
    int sign2 = sign(CrossZ2);
    
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
    
    Vector2f CD = D - C;         
    Vector2f CA = A - C;      
    Vector2f CB = B - C;         
    
    float CrossZ3 = Vector2f::Cross(CD, CA);  
    float CrossZ4 = Vector2f::Cross(CD, CB);
    
    int sign3 = sign(CrossZ3);
    int sign4 = sign(CrossZ4);
    
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
    Vector2f* intersectionPoint) 
{
    return checkIntersection(seg1.point1, seg1.point2,
                             seg2.point1, seg2.point2,
                             intersectionPoint);
}

bool IntersectionSegments2D::checkIntersection(
    const Segment& segment,
    const Vector2f& C, const Vector2f& D,
    Vector2f* intersectionPoint) 
{
    return checkIntersection(segment.point1, segment.point2,
                             C, D, intersectionPoint);
}

bool IntersectionSegments2D::checkIntersection(
    const Vector2f& A, const Vector2f& B,
    const Segment& segment,
    Vector2f* intersectionPoint) 
{
    return checkIntersection(A, B,
                             segment.point1, segment.point2,
                             intersectionPoint);
}

bool IntersectionSegments2D::checkPointOnSegment(
    const Vector2f& point, const Segment& segment) 
{
    return isPointOnSegment(point, segment.point1, segment.point2);
}

bool IntersectionSegments2D::getIntersectionPoint(
    const Segment& seg1, const Segment& seg2, Vector2f& outPoint) 
{
    return checkIntersection(seg1, seg2, &outPoint);
}