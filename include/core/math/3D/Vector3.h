#pragma once

#include <ostream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Vector.h"

template<typename T = float>
struct Vector3Impl : public Vector {
    T x, y, z;
    
    constexpr Vector3Impl() : x(0), y(0), z(0) {}
    constexpr Vector3Impl(T x, T y, T z) : x(x), y(y), z(z) {}
    
    constexpr T magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    constexpr T sqrMagnitude() const {
        return x * x + y * y + z * z;
    }
    
    constexpr Vector3Impl normalized() const {
        T mag = magnitude();
        if (mag > static_cast<T>(0.00001)) {
            return Vector3Impl(x / mag, y / mag, z / mag);
        }
        return Vector3Impl(0, 0, 0);
    }
    
    constexpr void normalize() {
        T mag = magnitude();
        if (mag > static_cast<T>(0.00001)) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }
    
    constexpr T distanceTo(const Vector3Impl& other) const {
        return (*this - other).magnitude();
    }
    
    constexpr T dot(const Vector3Impl& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    constexpr Vector3Impl cross(const Vector3Impl& other) const {
        return Vector3Impl(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    constexpr Vector3Impl clampMagnitude(T maxLength) const {
        T mag = magnitude();
        if (mag > maxLength) {
            return normalized() * maxLength;
        }
        return *this;
    }
    
    constexpr Vector3Impl reflect(const Vector3Impl& normal) const {
        return *this - normal * (static_cast<T>(2.0) * this->dot(normal));
    }
    
    static constexpr Vector3Impl lerp(const Vector3Impl& a, const Vector3Impl& b, T t) {
        t = (t < 0) ? 0 : (t > 1) ? 1 : t;
        return a + (b - a) * t;
    }
    
    static constexpr Vector3Impl moveTowards(const Vector3Impl& current, const Vector3Impl& target, T maxDistanceDelta) {
        Vector3Impl toTarget = target - current;
        T dist = toTarget.magnitude();
        
        if (dist <= maxDistanceDelta || dist == 0) {
            return target;
        }
        
        return current + toTarget / dist * maxDistanceDelta;
    }
    
    static constexpr T angle(const Vector3Impl& from, const Vector3Impl& to) {
        T dot = from.dot(to);
        T mags = from.magnitude() * to.magnitude();
        if (mags < static_cast<T>(0.00001)) return 0;
        return std::acos(dot / mags);
    }
    
    static constexpr Vector3Impl Zero() {
        return Vector3Impl(0, 0, 0);
    }
    
    static constexpr Vector3Impl One() {
        return Vector3Impl(1, 1, 1);
    }
    
    static constexpr Vector3Impl Forward() {
        return Vector3Impl(0, 0, 1);
    }
    
    static constexpr Vector3Impl Back() {
        return Vector3Impl(0, 0, -1);
    }
    
    static constexpr Vector3Impl Up() {
        return Vector3Impl(0, 1, 0);
    }
    
    static constexpr Vector3Impl Down() {
        return Vector3Impl(0, -1, 0);
    }
    
    static constexpr Vector3Impl Right() {
        return Vector3Impl(1, 0, 0);
    }
    
    static constexpr Vector3Impl Left() {
        return Vector3Impl(-1, 0, 0);
    }
    
    constexpr Vector3Impl operator-() const {
        return Vector3Impl(-x, -y, -z);
    }
    
    constexpr Vector3Impl& operator+=(const Vector3Impl& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    constexpr Vector3Impl& operator-=(const Vector3Impl& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    constexpr Vector3Impl& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    constexpr Vector3Impl& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    friend constexpr bool operator==(const Vector3Impl& a, const Vector3Impl& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    
    friend constexpr bool operator!=(const Vector3Impl& a, const Vector3Impl& b) {
        return !(a == b);
    }
    
    friend constexpr Vector3Impl operator+(const Vector3Impl& a, const Vector3Impl& b) {
        return Vector3Impl(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    
    friend constexpr Vector3Impl operator-(const Vector3Impl& a, const Vector3Impl& b) {
        return Vector3Impl(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    
    friend constexpr Vector3Impl operator*(const Vector3Impl& v, T scalar) {
        return Vector3Impl(v.x * scalar, v.y * scalar, v.z * scalar);
    }
    
    friend constexpr Vector3Impl operator*(T scalar, const Vector3Impl& v) {
        return v * scalar;
    }
    
    friend constexpr Vector3Impl operator/(const Vector3Impl& v, T scalar) {
        return Vector3Impl(v.x / scalar, v.y / scalar, v.z / scalar);
    }
    
    static std::string ToString(const Vector3Impl& v) {
        std::ostringstream oss;
        oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return oss.str();
    }
    
    static Vector3Impl FromString(const std::string& str) {
        T x, y, z;
        char c1, c2, c3, c4;
        std::istringstream iss(str);
        if (iss >> c1 >> x >> c2 >> y >> c3 >> z >> c4) {
            if (c1 == '(' && c2 == ',' && c3 == ',' && c4 == ')') {
                return Vector3Impl(x, y, z);
            }
        }
        return Vector3Impl(0, 0, 0);
    }
    
    using Float = Vector3Impl<float>;
};

using Vector3f = Vector3Impl<float>;
using Vector3d = Vector3Impl<double>;
using Vector3i = Vector3Impl<int>;
using Vector3 = Vector3Impl<float>;