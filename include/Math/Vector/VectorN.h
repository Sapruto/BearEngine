#pragma once

#include "BaseVector.h"

template <typename T, int Count>
struct VectorN : BaseVectorAlias<VectorN<T, Count>, T, Count> {
    T data[Count];

    constexpr VectorN() : data{} {}
    constexpr VectorN(const T (&arr)[Count]) : data{} {
        for (int i = 0; i < Count; i++) data[i] = arr[i];
    }

    constexpr T& operator[](int i) { return data[i]; }
    constexpr const T& operator[](int i) const { return data[i]; }
};