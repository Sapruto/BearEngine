#pragma once

#include "BaseVector.h"

template <typename T = float>
struct Vector4 : BaseVectorAlias<Vector4<T>, T, 4> {
    union {
        T data[4];
        struct { T x, y, z, w; };
    };

    constexpr Vector4() : data{} {}
    constexpr Vector4(T _x, T _y, T _z, T _w) : data{ _x, _y, _z, _w } {}

    constexpr T& operator[](int i) { return data[i]; }
    constexpr const T& operator[](int i) const { return data[i]; }
};

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;
using Vector4i = Vector4<int>;