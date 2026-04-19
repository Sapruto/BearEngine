#include "IntersectionSegments3D.h"
#include <cmath>
#include <algorithm>

int IntersectionSegments3D::Sign(float value, float epsilon) {
    if (value > epsilon) return 1;
    if (value < -epsilon) return -1;
    return 0;
}

bool IntersectionSegments3D::IsPointOnSegment(const Vector3& point, 
                                              const Vector3& segStart, 
                                              const Vector3& segEnd) {
    Vector3 segDir = segEnd - segStart;
    Vector3 pointDir = point - segStart;
    
    Vector3 cross = segDir.cross(pointDir);
    if (std::abs(cross.magnitude()) > 1e-6f) return false;
    
    float dot = segDir.dot(pointDir);
    if (dot < 0 || dot > segDir.sqrMagnitude()) return false;
    
    return true;
}

Vector3 IntersectionSegments3D::CalculateIntersectionPoint(
    const Vector3& A, const Vector3& B,
    const Vector3& C, const Vector3& D) {
    
    Vector3 AB = B - A;
    Vector3 CD = D - C;
    Vector3 AC = C - A;
    
    Vector3 crossAB_CD = AB.cross(CD);
    float denom = crossAB_CD.magnitude();
    
    if (std::abs(denom) < 1e-6f) {
        return Vector3(0, 0, 0);
    }
    
    Vector3 crossAC_CD = AC.cross(CD);
    float t = crossAC_CD.magnitude() / denom;
    
    Vector3 crossAC_AB = AC.cross(AB);
    float u = crossAC_AB.magnitude() / denom;
    
    if (Sign(AC.cross(CD).dot(AB.cross(CD))) < 0) t = -t;
    if (Sign(AC.cross(AB).dot(AB.cross(CD))) > 0) u = -u;
    
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return A + AB * t;
    }
    
    return Vector3(0, 0, 0);
}

bool IntersectionSegments3D::CheckIntersection(
    const Vector3& A, const Vector3& B,
    const Vector3& C, const Vector3& D, 
    Vector3* intersectionPoint) {
    
    Vector3 AB = B - A;
    Vector3 CD = D - C;
    Vector3 AC = C - A;
    
    Vector3 crossAB_CD = AB.cross(CD);
    
    if (std::abs(crossAB_CD.magnitude()) < 1e-6f) {
        Vector3 crossAC_AB = AC.cross(AB);
        if (std::abs(crossAC_AB.magnitude()) > 1e-6f) return false;
        
        float t0 = AC.dot(AB) / AB.sqrMagnitude();
        float t1 = (D - A).dot(AB) / AB.sqrMagnitude();
        
        if (t0 > 1.0f || t1 < 0.0f) return false;
        
        if (intersectionPoint) {
            float t = std::max(0.0f, std::min(t0, 1.0f));
            *intersectionPoint = A + AB * t;
        }
        return true;
    }
    
    float denom = crossAB_CD.magnitude();
    float t = AC.cross(CD).magnitude() / denom;
    float u = AC.cross(AB).magnitude() / denom;
    
    if (Sign(AC.cross(CD).dot(crossAB_CD)) < 0) t = -t;
    if (Sign(AC.cross(AB).dot(crossAB_CD)) > 0) u = -u;
    
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        if (intersectionPoint) {
            *intersectionPoint = A + AB * t;
        }
        return true;
    }
    
    return false;
}

bool IntersectionSegments3D::CheckIntersection(
    const Segment3D& seg1, const Segment3D& seg2,
    Vector3* intersectionPoint) {
    
    return CheckIntersection(seg1.point1, seg1.point2,
                             seg2.point1, seg2.point2,
                             intersectionPoint);
}

bool IntersectionSegments3D::CheckIntersection(
    const Segment3D& segment,
    const Vector3& C, const Vector3& D,
    Vector3* intersectionPoint) {
    
    return CheckIntersection(segment.point1, segment.point2,
                             C, D,
                             intersectionPoint);
}

bool IntersectionSegments3D::CheckIntersection(
    const Vector3& A, const Vector3& B,
    const Segment3D& segment,
    Vector3* intersectionPoint) {
    
    return CheckIntersection(A, B,
                             segment.point1, segment.point2,
                             intersectionPoint);
}

bool IntersectionSegments3D::IsPointOnSegment(const Vector3& point, 
                                               const Segment3D& segment) {
    return IsPointOnSegment(point, segment.point1, segment.point2);
}

bool IntersectionSegments3D::Intersects(const Segment3D& seg1, const Segment3D& seg2) {
    return CheckIntersection(seg1, seg2, nullptr);
}

bool IntersectionSegments3D::GetIntersectionPoint(
    const Segment3D& seg1, const Segment3D& seg2, Vector3& outPoint) {
    
    return CheckIntersection(seg1, seg2, &outPoint);
}