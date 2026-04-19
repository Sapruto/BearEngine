#include "Vector2.h"

#include <iostream>
#include <cmath>

float Vector2::magnitude() const {
    return sqrtf(x * x + y * y);
}

float Vector2::sqrMagnitude() const {
    return x * x + y * y;
}

Vector2 Vector2::normalized() const {
    float mag = magnitude();
    if (mag > 0.00001f) {
        return Vector2(x / mag, y / mag);
    }
    return Vector2(0, 0);
}

float Vector2::distanceTo(const Vector2& other) const {
    return (*this - other).magnitude();
}

float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

Vector2 Vector2::clampMagnitude(float maxLength) const {
    float mag = magnitude();
    if (mag > maxLength) {
        return normalized() * maxLength;
    }
    return *this;
}

Vector2 Vector2::reflect(const Vector2& normal) const {
    return *this - normal * (2.0f * this->dot(normal));
}

Vector2 Vector2::lerp(const Vector2& a, const Vector2& b, float t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    return a + (b - a) * t;
}

Vector2 Vector2::moveTowards(const Vector2& current, const Vector2& target, 
                            float maxDistanceDelta) {
    Vector2 toTarget = target - current;
    float dist = toTarget.magnitude();
    
    if (dist <= maxDistanceDelta || dist == 0) {
        return target;
    }
    
    return current + toTarget / dist * maxDistanceDelta;
}

Vector2 Vector2::zero() { return Vector2(0, 0); }
Vector2 Vector2::one() { return Vector2(1, 1); }
Vector2 Vector2::up() { return Vector2(0, 1); }
Vector2 Vector2::right() { return Vector2(1, 0); }


float Vector2::angle() const {
    return atan2f(y, x);
}
Vector2 Vector2::fromAngle(float angleRad) {
    return Vector2(cosf(angleRad), sinf(angleRad));
}

Vector2 Vector2::rotated(float angleRad) const {
    float cosA = cosf(angleRad);
    float sinA = sinf(angleRad);
    return Vector2(
        x * cosA - y * sinA,
        x * sinA + y * cosA
    );
}

Vector2 Vector2::rotateTowards(const Vector2& current, const Vector2& target,
                            float maxRadiansDelta) {
    float currentAngle = current.angle();
    float targetAngle = target.angle();
    
    float angleDiff = targetAngle - currentAngle;
    while (angleDiff > 3.1415926535f) angleDiff -= 6.283185307f;
    while (angleDiff < -3.1415926535f) angleDiff += 6.283185307f;
    
    if (angleDiff > maxRadiansDelta) angleDiff = maxRadiansDelta;
    else if (angleDiff < -maxRadiansDelta) angleDiff = -maxRadiansDelta;
    
    return current.rotated(angleDiff);
}

float Vector2::cross(const Vector2& a, const Vector2& b){
    return a.x * b.y - a.y * b.x;
}