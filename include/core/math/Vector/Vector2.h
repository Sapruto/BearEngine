#pragma once

#include <cmath>
#include "BaseVector.h"

template <typename T = float>
struct Vector2 : BaseVectorAlias<Vector2<T>, T, 2> {
    union {
        T data[2];
        struct { T x, y; };
    };

    constexpr Vector2() : data{} {}
    constexpr Vector2(T _x, T _y) : data{ _x, _y } {}

    constexpr T& operator[](int i) { return data[i]; }
    constexpr const T& operator[](int i) const { return data[i]; }

    static T angle(const Vector2& v) { return std::atan2(v.y, v.x); }
    static Vector2 fromAngle(T a) { return { std::cos(a), std::sin(a) }; }

    static constexpr T cross(const Vector2& a, const Vector2& b) {
        return a.x * b.y - a.y * b.x;
    }

    static constexpr Vector2 Up() { return { T(0), T(1) }; }
    static constexpr Vector2 Right() { return { T(1), T(0) }; }
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;