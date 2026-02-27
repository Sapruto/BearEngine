#pragma once

#include <iostream>
#include <cmath>

class Vector2{
public:
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    
    float magnitude() const;
    
    float sqrMagnitude() const ;
    
    Vector2 normalized() const;
    
    float distanceTo(const Vector2& other) const;
    
    float dot(const Vector2& other) const;
    
    Vector2 clampMagnitude(float maxLength) const;
    
    Vector2 reflect(const Vector2& normal) const;
    
    static Vector2 lerp(const Vector2& a, const Vector2& b, float t);
    
    static Vector2 moveTowards(const Vector2& current, const Vector2& target, 
                              float maxDistanceDelta);
    
    static Vector2 zero();
    static Vector2 one();
    static Vector2 up();
    static Vector2 right();

    
    float angle() const;
    static Vector2 fromAngle(float angleRad);
    
    Vector2 rotated(float angleRad) const;
    
    static Vector2 rotateTowards(const Vector2& current, const Vector2& target,
                                float maxRadiansDelta);

    static float cross(const Vector2& a, const Vector2& b);
    
    Vector2 operator-() const { return Vector2(-x, -y); }
    
    Vector2& operator+=(const Vector2& other) {
        x += other.x; y += other.y;
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }
    
    Vector2& operator*=(float scalar) {
        x *= scalar; y *= scalar;
        return *this;
    }
    
    Vector2& operator/=(float scalar) {
        x /= scalar; y /= scalar;
        return *this;
    }

    static Vector2 Zero() { return Vector2(0, 0); }
    static Vector2 One() { return Vector2(1, 1); }
    
    friend Vector2 operator+(const Vector2& a, const Vector2& b) {
        return Vector2(a.x + b.x, a.y + b.y);
    }
    
    friend Vector2 operator-(const Vector2& a, const Vector2& b) {
        return Vector2(a.x - b.x, a.y - b.y);
    }
    
    friend Vector2 operator*(const Vector2& v, float scalar) {
        return Vector2(v.x * scalar, v.y * scalar);
    }
    
    friend Vector2 operator*(float scalar, const Vector2& v) {
        return v * scalar;
    }
    
    friend Vector2 operator/(const Vector2& v, float scalar) {
        return Vector2(v.x / scalar, v.y / scalar);
    }
    
    friend bool operator==(const Vector2& a, const Vector2& b) {
        return a.x == b.x && a.y == b.y;
    }
    friend bool operator>=(const Vector2& a, const Vector2& b) {
        return a.x >= b.x && a.y >= b.y;
    }
    friend bool operator<=(const Vector2& a, const Vector2& b) {
        return a.x <= b.x && a.y <= b.y;
    }
    friend bool operator>(const Vector2& a, const Vector2& b) {
        return a.x > b.x && a.y > b.y;
    }
    friend bool operator<(const Vector2& a, const Vector2& b) {
        return a.x < b.x && a.y < b.y;
    }
    
    friend bool operator!=(const Vector2& a, const Vector2& b) {
        return !(a == b);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};