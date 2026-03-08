#pragma once

#include <ostream>
#include <cmath>
#include <iostream>  
#include "Vector.h"

class Vector3 : public Vector{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    void normalize();
    
    float magnitude() const;
    float sqrMagnitude() const;
    float distanceTo(const Vector3& other) const;
    float dot(const Vector3& other) const;
    
    Vector3 clampMagnitude(float maxLength) const;
    Vector3 reflect(const Vector3& normal) const;
    Vector3 normalized() const;
    Vector3 cross(const Vector3& other) const;

    static float angle(const Vector3& from, const Vector3& to);
    
    static Vector3 lerp(const Vector3& a, const Vector3& b, float t);
    
    static Vector3 moveTowards(const Vector3& current, const Vector3& target, 
                              float maxDistanceDelta);

    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Forward;
    static const Vector3 Back;
    static const Vector3 Up;
    static const Vector3 Down;
    static const Vector3 Right;
    static const Vector3 Left;

    Vector3 operator-() const { return Vector3(-x, -y, -z); }
    
    Vector3& operator+=(const Vector3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    
    Vector3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    
    Vector3& operator/=(float scalar) {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }
    
    friend bool operator==(const Vector3& a, const Vector3& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    friend bool operator>=(const Vector3& a, const Vector3& b) {
        return a.x >= b.x && a.y >= b.y && a.z >= b.z;
    }
    friend bool operator<=(const Vector3& a, const Vector3& b) {
        return a.x <= b.x && a.y <= b.y && a.z <= b.z;
    }
    friend bool operator>(const Vector3& a, const Vector3& b) {
        return a.x > b.x && a.y > b.y && a.z > b.z;
    }
    friend bool operator<(const Vector3& a, const Vector3& b) {
        return a.x < b.x && a.y < b.y && a.z < b.z;
    }
    
    friend bool operator!=(const Vector3& a, const Vector3& b) {
        return !(a == b);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v.x << ", " << v.y << "," << v.z << ")";
        return os;
    }
};

inline Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator*(const Vector3& v, float scalar) {
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

inline Vector3 operator*(float scalar, const Vector3& v) {
    return v * scalar;
}

inline Vector3 operator/(const Vector3& v, float scalar) {
    return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
}