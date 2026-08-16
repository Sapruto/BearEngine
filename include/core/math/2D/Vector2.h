#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "Vector.h"

template<typename T = float>
struct Vector2Impl : public Vector {
    T x, y;
    
    constexpr Vector2Impl() : x(0), y(0) {}
    constexpr Vector2Impl(T x, T y) : x(x), y(y) {}
    
    constexpr T magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    constexpr T sqrMagnitude() const {
        return x * x + y * y;
    }
    
    constexpr Vector2Impl normalized() const {
        T mag = magnitude();
        if (mag > static_cast<T>(0.00001)) {
            return Vector2Impl(x / mag, y / mag);
        }
        return Vector2Impl(0, 0);
    }
    
    constexpr T distanceTo(const Vector2Impl& other) const {
        return (*this - other).magnitude();
    }
    
    constexpr T dot(const Vector2Impl& other) const {
        return x * other.x + y * other.y;
    }
    
    constexpr Vector2Impl clampMagnitude(T maxLength) const {
        T mag = magnitude();
        if (mag > maxLength) {
            return normalized() * maxLength;
        }
        return *this;
    }
    
    constexpr Vector2Impl reflect(const Vector2Impl& normal) const {
        return *this - normal * (static_cast<T>(2.0) * this->dot(normal));
    }
    
    static constexpr Vector2Impl lerp(const Vector2Impl& a, const Vector2Impl& b, T t) {
        t = (t < 0) ? 0 : (t > 1) ? 1 : t;
        return a + (b - a) * t;
    }
    
    static constexpr Vector2Impl moveTowards(const Vector2Impl& current, const Vector2Impl& target, T maxDistanceDelta) {
        Vector2Impl toTarget = target - current;
        T dist = toTarget.magnitude();
        
        if (dist <= maxDistanceDelta || dist == 0) {
            return target;
        }
        
        return current + toTarget / dist * maxDistanceDelta;
    }
    
    static constexpr Vector2Impl Zero() { 
        return Vector2Impl(0, 0); 
    }
    
    static constexpr Vector2Impl One() { 
        return Vector2Impl(1, 1); 
    }
    
    static constexpr Vector2Impl Up() { 
        return Vector2Impl(0, 1); 
    }
    
    static constexpr Vector2Impl Right() { 
        return Vector2Impl(1, 0); 
    }
    
    static constexpr T Angle(const Vector2Impl& v) {
        return std::atan2(v.y, v.x);
    }
    
    static constexpr Vector2Impl FromAngle(T angleRad) {
        return Vector2Impl(std::cos(angleRad), std::sin(angleRad));
    }
    
    static constexpr Vector2Impl RotateTowards(const Vector2Impl& current, const Vector2Impl& target, T maxRadiansDelta) {
        T currentAngle = Angle(current);
        T targetAngle = Angle(target);
        
        T angleDiff = targetAngle - currentAngle;
        const T PI = static_cast<T>(3.14159265358979323846);
        const T TWO_PI = static_cast<T>(6.28318530717958647692);
        
        while (angleDiff > PI) angleDiff -= TWO_PI;
        while (angleDiff < -PI) angleDiff += TWO_PI;
        
        if (angleDiff > maxRadiansDelta) angleDiff = maxRadiansDelta;
        else if (angleDiff < -maxRadiansDelta) angleDiff = -maxRadiansDelta;
        
        return Vector2Impl(
            current.x * std::cos(angleDiff) - current.y * std::sin(angleDiff),
            current.x * std::sin(angleDiff) + current.y * std::cos(angleDiff)
        );
    }
    
    static constexpr T Cross(const Vector2Impl& a, const Vector2Impl& b) {
        return a.x * b.y - a.y * b.x;
    }
    
    constexpr Vector2Impl operator-() const {
        return Vector2Impl(-x, -y);
    }
    
    constexpr Vector2Impl& operator+=(const Vector2Impl& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    constexpr Vector2Impl& operator-=(const Vector2Impl& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    constexpr Vector2Impl& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    constexpr Vector2Impl& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    
    friend constexpr bool operator==(const Vector2Impl& a, const Vector2Impl& b) {
        return a.x == b.x && a.y == b.y;
    }
    
    friend constexpr bool operator!=(const Vector2Impl& a, const Vector2Impl& b) {
        return !(a == b);
    }
    
    friend constexpr Vector2Impl operator+(const Vector2Impl& a, const Vector2Impl& b) {
        return Vector2Impl(a.x + b.x, a.y + b.y);
    }
    
    friend constexpr Vector2Impl operator-(const Vector2Impl& a, const Vector2Impl& b) {
        return Vector2Impl(a.x - b.x, a.y - b.y);
    }
    
    friend constexpr Vector2Impl operator*(const Vector2Impl& v, T scalar) {
        return Vector2Impl(v.x * scalar, v.y * scalar);
    }
    
    friend constexpr Vector2Impl operator*(T scalar, const Vector2Impl& v) {
        return v * scalar;
    }
    
    friend constexpr Vector2Impl operator/(const Vector2Impl& v, T scalar) {
        return Vector2Impl(v.x / scalar, v.y / scalar);
    }
    
    static std::string ToString(const Vector2Impl& v) {
        std::ostringstream oss;
        oss << "(" << v.x << ", " << v.y << ")";
        return oss.str();
    }
    
    static Vector2Impl FromString(const std::string& str) {
        T x, y;
        char c1, c2, c3;
        std::istringstream iss(str);
        if (iss >> c1 >> x >> c2 >> y >> c3) {
            if (c1 == '(' && c2 == ',' && c3 == ')') {
                return Vector2Impl(x, y);
            }
        }
        return Vector2Impl(0, 0);
    }
    
    using Float = Vector2Impl<float>;
};

using Vector2f = Vector2Impl<float>;
using Vector2d = Vector2Impl<double>;
using Vector2i = Vector2Impl<int>;
using Vector2 = Vector2Impl<float>;