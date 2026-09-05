#pragma once

#include "Vector.h"

template<typename T, int Rows, int Cols>
struct BaseMatrix {
private:
    T data[Rows * Cols];

public:
    constexpr BaseMatrix() {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] = T();
        }
    }
    
    explicit BaseMatrix(const T newData[Rows * Cols]) {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] = newData[i];
        }
    }

    constexpr BaseMatrix& operator+=(const BaseMatrix& other) {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] += other.data[i];
        }
        return *this;
    }
    
    constexpr BaseMatrix& operator-=(const BaseMatrix& other) {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] -= other.data[i];
        }
        return *this;
    }
    
    constexpr BaseMatrix& operator*=(T scalar) {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] *= scalar;
        }
        return *this;
    }
    
    constexpr BaseMatrix& operator/=(T scalar) {
        for (int i = 0; i < Rows * Cols; i++) {
            data[i] /= scalar;
        }
        return *this;
    }

    constexpr BaseMatrix operator+(const BaseMatrix& other) const {
        BaseMatrix result;
        for (int i = 0; i < Rows * Cols; i++) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }
    
    constexpr BaseMatrix operator-(const BaseMatrix& other) const {
        BaseMatrix result;
        for (int i = 0; i < Rows * Cols; i++) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }
    
    constexpr bool operator==(const BaseMatrix& other) const {
        for (int i = 0; i < Rows * Cols; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    constexpr BaseMatrix<T, Cols, Rows> transpose() const {
        BaseMatrix<T, Cols, Rows> result;
        for (int i = 0; i < Rows; i++) {
            for (int j = 0; j < Cols; j++) {
                result(j, i) = data[i * Cols + j];
            }
        }
        return result;
    }

    template<typename U, int Rows1, int Cols1, int Cols2>
    static constexpr auto multiply(const BaseMatrix<U, Rows1, Cols1>& first, const BaseMatrix<U, Cols1, Cols2>& second) {
        BaseMatrix<U, Rows1, Cols2> result;
        for (int i = 0; i < Rows1; i++) {
            for (int j = 0; j < Cols2; j++) {
                U sum = U();
                for (int k = 0; k < Cols1; k++) {
                    sum += first(i, k) * second(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    constexpr BaseMatrix adamMultiply(const BaseMatrix& other) const {
        BaseMatrix result;
        for (int i = 0; i < Rows * Cols; i++) {
            result.data[i] = data[i] * other.data[i];
        }
        return result;
    }
    
    template<typename U, int Rows1, int Cols1, int Rows2, int Cols2>
    static constexpr auto kroneckerMultiply(const BaseMatrix<U, Rows1, Cols1>& a, const BaseMatrix<U, Rows2, Cols2>& b) {
        BaseMatrix<U, Rows1 * Rows2, Cols1 * Cols2> result;
        
        for (int i = 0; i < Rows1; i++) {
            for (int j = 0; j < Cols1; j++) {
                for (int k = 0; k < Rows2; k++) {
                    for (int l = 0; l < Cols2; l++) {
                        result(i * Rows2 + k, j * Cols2 + l) = a(i, j) * b(k, l);
                    }
                }
            }
        }
        return result;
    }

    constexpr T* GetData() { return data; }
    constexpr const T* GetData() const { return data; }
    constexpr T& operator[](int index) { return data[index]; }
    constexpr const T& operator[](int index) const { return data[index]; }
    constexpr const T& operator()(int row, int col) const { 
        return data[row * Cols + col];
    }
    T& operator()(int row, int col) { 
        return data[row * Cols + col];
    }
    constexpr int Size() const { return Rows * Cols; }

    static constexpr BaseMatrix Identity() {
        static_assert(Rows == Cols, "Identity matrix only defined for square matrices");
        BaseMatrix result;
        for (int i = 0; i < Rows && i < Cols; i++) {
            result(i, i) = T(1);
        }
        return result;
    }
    
    static constexpr BaseMatrix Zero() {
        return BaseMatrix();
    }
};

/*
template<typename T, int Rows, int Cols>
constexpr Vector<T, Rows> operator*(const BaseMatrix<T, Rows, Cols>& m, const Vector<T, Cols>& v) {
    Vector<T, Rows> result;
    for (int i = 0; i < Rows; i++) {
        T sum = T();
        for (int j = 0; j < Cols; j++) {
            sum += m(i, j) * v[j];
        }
        result[i] = sum;
    }
    return result;
}
*/