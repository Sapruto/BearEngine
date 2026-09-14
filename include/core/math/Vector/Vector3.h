#pragma once

#include <cmath>
#include "BaseVector.h"

template <typename T = float>
struct Vector3 : BaseVectorAlias<Vector3<T>, T, 3> {
    union {
        T data[3];
        struct { T x, y, z; };
    };

    constexpr Vector3() : data{} {}
    constexpr Vector3(T _x, T _y, T _z) : data{ _x, _y, _z } {}

    constexpr T& operator[](int i) { return data[i]; }
    constexpr const T& operator[](int i) const { return data[i]; }

    constexpr Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    static T angle(const Vector3& from, const Vector3& to) {
        T d = from.dot(to);
        T mags = from.magnitude() * to.magnitude();
        if (mags < static_cast<T>(0.00001)) return 0;
        return std::acos(d / mags);
    }

    static constexpr Vector3 Forward() { return Vector3(0, 0, 1); }
    static constexpr Vector3 Back() { return Vector3(0, 0, -1); }
    static constexpr Vector3 Up() { return Vector3(0, 1, 0); }
    static constexpr Vector3 Down() { return Vector3(0, -1, 0); }
    static constexpr Vector3 Right() { return Vector3(1, 0, 0); }
    static constexpr Vector3 Left() { return Vector3(-1, 0, 0); }
};

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3i = Vector3<int>;