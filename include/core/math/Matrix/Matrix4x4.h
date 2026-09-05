#pragma once

#include "Matrix/BaseMatrix.h"
#include "Vector3.h"
#include <cmath>

template<typename T>
struct Matrix4x4 : public BaseMatrix<T, 4, 4> {
private:
    constexpr void getMinor3x3(const T* d, int row, int col, T* minor) const {
        int idx = 0;
        for (int i = 0; i < 4; i++) {
            if (i == row) continue;
            for (int j = 0; j < 4; j++) {
                if (j == col) continue;
                minor[idx++] = d[i * 4 + j];
            }
        }
    }

    static constexpr T determinant3x3(const T* d) {
        return d[0] * (d[4] * d[8] - d[5] * d[7])
                - d[1] * (d[3] * d[8] - d[5] * d[6])
                + d[2] * (d[3] * d[7] - d[4] * d[6]);
    }

public:
    using Base = BaseMatrix<T, 4, 4>;
    using Base::BaseMatrix;

    Matrix4x4(const BaseMatrix<T, 4, 4>& other) : BaseMatrix<T, 4, 4>(other) {}

    constexpr T determinant() const {
        const T* d = this->GetData();
        
        T minor00[9], minor01[9], minor02[9], minor03[9];
        getMinor3x3(d, 0, 0, minor00);
        getMinor3x3(d, 0, 1, minor01);
        getMinor3x3(d, 0, 2, minor02);
        getMinor3x3(d, 0, 3, minor03);
        
        return d[0] * determinant3x3(minor00)
                - d[1] * determinant3x3(minor01)
                + d[2] * determinant3x3(minor02)
                - d[3] * determinant3x3(minor03);
    }
    
    BaseMatrix<T, 4, 4> inverse() const {
        T det = determinant();
        if (det == T(0)) return BaseMatrix<T, 4, 4>::Identity();
        
        BaseMatrix<T, 4, 4> inv;
        const T* d = this->GetData();
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                T minor[9];
                getMinor3x3(d, i, j, minor);
                T cofactor = determinant3x3(minor);
                if ((i + j) % 2 != 0) cofactor = -cofactor;
                inv(j, i) = cofactor / det;
            }
        }
        
        return inv;
    }

    static Matrix4x4 Identity() {
        Matrix4x4 m;
        for (int i = 0; i < 4; i++) m(i, i) = T(1);
        return m;
    }

    static Matrix4x4<T> lookAt(const Vector3Impl<T>& eye, const Vector3Impl<T>& target, const Vector3Impl<T>& up) {
        Vector3Impl<T> forward = (target - eye).normalized();
        Vector3Impl<T> right = forward.cross(up).normalized();
        Vector3Impl<T> newUp = right.cross(forward);
        
        Matrix4x4<T> view;
        
        view(0, 0) = right.x;
        view(0, 1) = newUp.x;
        view(0, 2) = -forward.x;
        view(0, 3) = 0;
        
        view(1, 0) = right.y;
        view(1, 1) = newUp.y;
        view(1, 2) = -forward.y;
        view(1, 3) = 0;
        
        view(2, 0) = right.z;
        view(2, 1) = newUp.z;
        view(2, 2) = -forward.z;
        view(2, 3) = 0;
        
        view(3, 0) = -right.dot(eye);
        view(3, 1) = -newUp.dot(eye);
        view(3, 2) = forward.dot(eye);
        view(3, 3) = 1;
        
        return view;
    }
    
    static Matrix4x4<T> perspective(T fovDegrees, T aspect, T near, T far) {
        T fovRad = fovDegrees * 3.14159265359f / 180.0f;
        T tanHalfFov = std::tan(fovRad / 2.0f);
        
        Matrix4x4<T> proj;
        
        proj(0, 0) = 1.0f / (aspect * tanHalfFov);
        proj(0, 1) = 0;
        proj(0, 2) = 0;
        proj(0, 3) = 0;
        
        proj(1, 0) = 0;
        proj(1, 1) = 1.0f / tanHalfFov;
        proj(1, 2) = 0;
        proj(1, 3) = 0;
        
        proj(2, 0) = 0;
        proj(2, 1) = 0;
        proj(2, 2) = -(far + near) / (far - near);
        proj(2, 3) = -1.0f;
        
        proj(3, 0) = 0;
        proj(3, 1) = 0;
        proj(3, 2) = -(2.0f * far * near) / (far - near);
        proj(3, 3) = 0;
        
        return proj;
    }

    static Matrix4x4<T> ortho(T left, T right, T bottom, T top, T near, T far) {
        Matrix4x4<T> m;
        m(0,0) = 2.0f / (right - left);
        m(1,1) = 2.0f / (top - bottom);
        m(2,2) = -2.0f / (far - near);
        m(3,0) = -(right + left) / (right - left);
        m(3,1) = -(top + bottom) / (top - bottom);
        m(3,2) = -(far + near) / (far - near);
        m(3,3) = 1.0f;
        return m;
    }
};

template<typename T>
Matrix4x4<T> operator*(const Matrix4x4<T>& a, const Matrix4x4<T>& b) {
    Matrix4x4<T> result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            T sum = T(0);
            for (int k = 0; k < 4; ++k) {
                sum += a(i, k) * b(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

template<typename T>
Vector3Impl<T> operator*(const Matrix4x4<T>& m, const Vector3Impl<T>& v) {
    Vector3Impl<T> result;
    result.x = m(0,0) * v.x + m(0,1) * v.y + m(0,2) * v.z + m(0,3);
    result.y = m(1,0) * v.x + m(1,1) * v.y + m(1,2) * v.z + m(1,3);
    result.z = m(2,0) * v.x + m(2,1) * v.y + m(2,2) * v.z + m(2,3);
    return result;
}

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;
using Matrix4x4i = Matrix4x4<int>;