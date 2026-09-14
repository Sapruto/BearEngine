#pragma once

#include "Matrix/BaseMatrix.h"

template<typename T>
struct Matrix3x3 : public BaseMatrixAlias<T, 3, 3> {
public:
    using Base = BaseMatrixAlias<T, 3, 3>;
    using Base::Base;

    constexpr T determinant() const {
        return (*this)(0,0) * ((*this)(1,1) * (*this)(2,2) - (*this)(1,2) * (*this)(2,1))
             - (*this)(0,1) * ((*this)(1,0) * (*this)(2,2) - (*this)(1,2) * (*this)(2,0))
             + (*this)(0,2) * ((*this)(1,0) * (*this)(2,1) - (*this)(1,1) * (*this)(2,0));
    }

    constexpr Matrix3x3 inverse() const {
        T det = determinant();
        if (det == T(0)) return Matrix3x3::Identity();

        Matrix3x3 inv;
        inv(0,0) = ((*this)(1,1) * (*this)(2,2) - (*this)(1,2) * (*this)(2,1)) / det;
        inv(0,1) = ((*this)(0,2) * (*this)(2,1) - (*this)(0,1) * (*this)(2,2)) / det;
        inv(0,2) = ((*this)(0,1) * (*this)(1,2) - (*this)(0,2) * (*this)(1,1)) / det;
        inv(1,0) = ((*this)(1,2) * (*this)(2,0) - (*this)(1,0) * (*this)(2,2)) / det;
        inv(1,1) = ((*this)(0,0) * (*this)(2,2) - (*this)(0,2) * (*this)(2,0)) / det;
        inv(1,2) = ((*this)(0,2) * (*this)(1,0) - (*this)(0,0) * (*this)(1,2)) / det;
        inv(2,0) = ((*this)(1,0) * (*this)(2,1) - (*this)(1,1) * (*this)(2,0)) / det;
        inv(2,1) = ((*this)(0,1) * (*this)(2,0) - (*this)(0,0) * (*this)(2,1)) / det;
        inv(2,2) = ((*this)(0,0) * (*this)(1,1) - (*this)(0,1) * (*this)(1,0)) / det;
        return inv;
    }

    static constexpr Matrix3x3 Identity() {
        Matrix3x3 m;
        for (int i = 0; i < 3; i++) m(i, i) = T(1);
        return m;
    }
};

using Matrix3x3f = Matrix3x3<float>;
using Matrix3x3d = Matrix3x3<double>;
using Matrix3x3i = Matrix3x3<int>;