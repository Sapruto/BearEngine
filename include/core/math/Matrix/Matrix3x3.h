#pragma once

#include "Matrix/BaseMatrix.h"

template<typename T>
struct Matrix3x3 : public BaseMatrix<T, 3, 3> {
public:
    using Base = BaseMatrix<T, 3, 3>;
    using Base::BaseMatrix;
    
    constexpr T determinant() const {
        const T* d = this->GetData();
        return d[0] * (d[4] * d[8] - d[5] * d[7])
                - d[1] * (d[3] * d[8] - d[5] * d[6])
                + d[2] * (d[3] * d[7] - d[4] * d[6]);
    }
    
    BaseMatrix<T, 3, 3> inverse() const {
        T det = determinant();
        if (det == T(0)) return BaseMatrix<T, 3, 3>::Identity();
        
        BaseMatrix<T, 3, 3> inv;
        const T* d = this->GetData();
        
        inv(0,0) = (d[4] * d[8] - d[5] * d[7]) / det;
        inv(0,1) = (d[2] * d[7] - d[1] * d[8]) / det;
        inv(0,2) = (d[1] * d[5] - d[2] * d[4]) / det;
        inv(1,0) = (d[5] * d[6] - d[3] * d[8]) / det;
        inv(1,1) = (d[0] * d[8] - d[2] * d[6]) / det;
        inv(1,2) = (d[2] * d[3] - d[0] * d[5]) / det;
        inv(2,0) = (d[3] * d[7] - d[4] * d[6]) / det;
        inv(2,1) = (d[1] * d[6] - d[0] * d[7]) / det;
        inv(2,2) = (d[0] * d[4] - d[1] * d[3]) / det;
        return inv;
    }
};

using Matrix3x3f = Matrix3x3<float>;
using Matrix3x3d = Matrix3x3<double>;
using Matrix3x3i = Matrix3x3<int>;