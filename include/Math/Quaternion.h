#pragma once

#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include "Math/Vector/Vector3.h"
#include "Math/Matrix/Matrix4x4.h"

template<typename T = float>
struct Quaternion {
    Vector3<T> ratioVec;
    T scalar;

    Quaternion() : ratioVec(Vector3<T>::Zero()), scalar(T(0)) {}
    constexpr Quaternion(Vector3<T> ratioVec, T scalar) 
        : ratioVec(ratioVec), scalar(scalar) {}

    static Quaternion Identity() {
        return Quaternion(Vector3<T>(0, 0, 0), T(1));
    }
    
    static Quaternion CreateRotation(const T& angle, const Vector3<T>& axis) {
        T halfA = angle / T(2);
        T sinA = std::sin(halfA);
        T cosA = std::cos(halfA);

        Vector3<T> NormalizedAxis = axis.Normalized();
        
        Quaternion q;
        q.ratioVec = NormalizedAxis * sinA;
        q.scalar = cosA;
        
        return q;
    }
    
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, T t) {
        T dot = a.scalar * b.scalar + a.ratioVec.Dot(b.ratioVec);
        Quaternion b2 = b;
        
        if (dot < T(0)) {
            b2 = -b;
            dot = -dot;
        }
        
        if (dot > T(0.9995)) {
            Quaternion result = a + (b2 - a) * t;
            return result.Normalized();
        }
        
        T theta = std::acos(dot);
        T sinTheta = std::sin(theta);
        T w1 = std::sin((T(1) - t) * theta) / sinTheta;
        T w2 = std::sin(t * theta) / sinTheta;
        
        return a * w1 + b2 * w2;
    }

    Quaternion Conjugate() const {
        return Quaternion(Vector3<T>(-ratioVec.x, -ratioVec.y, -ratioVec.z), scalar);
    }

    T Magnitude() const {
        return std::sqrt(scalar * scalar 
                        + ratioVec.x * ratioVec.x 
                        + ratioVec.y * ratioVec.y 
                        + ratioVec.z * ratioVec.z);
    }

    Quaternion Normalized() const {
        T mag = Magnitude();
        if (mag == T(0)) return Identity();
        return Quaternion(Vector3<T>(ratioVec.x / mag, 
                                        ratioVec.y / mag, 
                                        ratioVec.z / mag), 
                            scalar / mag);
    }

    Quaternion Inverse() const {
        T normSq = scalar * scalar 
                    + ratioVec.x * ratioVec.x 
                    + ratioVec.y * ratioVec.y 
                    + ratioVec.z * ratioVec.z;
        
        if (normSq == T(0)) return Identity();
        
        Quaternion conj = Conjugate();
        return Quaternion(Vector3<T>(conj.ratioVec.x / normSq,
                                        conj.ratioVec.y / normSq,
                                        conj.ratioVec.z / normSq),
                            conj.scalar / normSq);
    }

    Vector3<T> RotateVector(const Vector3<T>& vec) const {
        Vector3<T> vecPart = ratioVec;
        T scalarPart = scalar;
        
        Vector3<T> t = vecPart.Cross(vec) * T(2);
        return vec + t * scalarPart + vecPart.Cross(t);
    }

    Quaternion operator*(const Quaternion& other) const {
        Quaternion result;
        
        result.scalar = scalar * other.scalar 
                        - ratioVec.x * other.ratioVec.x 
                        - ratioVec.y * other.ratioVec.y 
                        - ratioVec.z * other.ratioVec.z;
        
        result.ratioVec.x = scalar * other.ratioVec.x 
                            + ratioVec.x * other.scalar 
                            + ratioVec.y * other.ratioVec.z 
                            - ratioVec.z * other.ratioVec.y;
        
        result.ratioVec.y = scalar * other.ratioVec.y 
                            - ratioVec.x * other.ratioVec.z 
                            + ratioVec.y * other.scalar 
                            + ratioVec.z * other.ratioVec.x;
        
        result.ratioVec.z = scalar * other.ratioVec.z 
                            + ratioVec.x * other.ratioVec.y 
                            - ratioVec.y * other.ratioVec.x 
                            + ratioVec.z * other.scalar;
        
        return result;
    }
    
    Quaternion operator+(const Quaternion& other) const {
        return Quaternion(Vector3<T>(ratioVec.x + other.ratioVec.x,
                                        ratioVec.y + other.ratioVec.y,
                                        ratioVec.z + other.ratioVec.z),
                            scalar + other.scalar);
    }
    
    Quaternion operator-(const Quaternion& other) const {
        return Quaternion(Vector3<T>(ratioVec.x - other.ratioVec.x,
                                        ratioVec.y - other.ratioVec.y,
                                        ratioVec.z - other.ratioVec.z),
                            scalar - other.scalar);
    }
    
    Quaternion operator*(const T& scale) const {
        return Quaternion(Vector3<T>(ratioVec.x * scale,
                                        ratioVec.y * scale,
                                        ratioVec.z * scale),
                            scalar * scale);
    }
    
    friend Quaternion operator*(const T& scale, const Quaternion& q) {
        return q * scale;
    }
    
    Quaternion operator-() const {
        return Quaternion(-ratioVec, -scalar);
    }
    
    std::string ToString() const {
        return std::to_string(ratioVec.x) + "," +
                std::to_string(ratioVec.y) + "," +
                std::to_string(ratioVec.z) + "," +
                std::to_string(scalar);
    }
    
    static Quaternion FromString(const std::string& str) {
        Quaternion result;
        
        std::stringstream ss(str);
        std::string token;
        
        if (std::getline(ss, token, ',')) {
            result.ratioVec.x = static_cast<T>(std::atof(token.c_str()));
        }
        if (std::getline(ss, token, ',')) {
            result.ratioVec.y = static_cast<T>(std::atof(token.c_str()));
        }
        if (std::getline(ss, token, ',')) {
            result.ratioVec.z = static_cast<T>(std::atof(token.c_str()));
        }
        if (std::getline(ss, token, ',')) {
            result.scalar = static_cast<T>(std::atof(token.c_str()));
        }
        
        return result;
    }

    static Quaternion<T> CreateFromMatrix(const Matrix4x4<T>& m) {
        Quaternion<T> q;
        T tr = m(0,0) + m(1,1) + m(2,2);
        if (tr > T(0)) {
            T s = T(0.5) / std::sqrt(tr + T(1));
            q.scalar     = T(0.25) / s;
            q.ratioVec.x = (m(2,1) - m(1,2)) * s;
            q.ratioVec.y = (m(0,2) - m(2,0)) * s;
            q.ratioVec.z = (m(1,0) - m(0,1)) * s;
        }
        else if (m(0,0) > m(1,1) && m(0,0) > m(2,2)) {
            T s = T(2) * std::sqrt(T(1) + m(0,0) - m(1,1) - m(2,2));
            q.scalar     = (m(2,1) - m(1,2)) / s;
            q.ratioVec.x = T(0.25) * s;
            q.ratioVec.y = (m(0,1) + m(1,0)) / s;
            q.ratioVec.z = (m(0,2) + m(2,0)) / s;
        }
        else if (m(1,1) > m(2,2)) {
            T s = T(2) * std::sqrt(T(1) + m(1,1) - m(0,0) - m(2,2));
            q.scalar     = (m(0,2) - m(2,0)) / s;
            q.ratioVec.x = (m(0,1) + m(1,0)) / s;
            q.ratioVec.y = T(0.25) * s;
            q.ratioVec.z = (m(1,2) + m(2,1)) / s;
        }
        else {
            T s = T(2) * std::sqrt(T(1) + m(2,2) - m(0,0) - m(1,1));
            q.scalar     = (m(1,0) - m(0,1)) / s;
            q.ratioVec.x = (m(0,2) + m(2,0)) / s;
            q.ratioVec.y = (m(1,2) + m(2,1)) / s;
            q.ratioVec.z = T(0.25) * s;
        }
        return q.Normalized();
    }

    Vector3<T> operator*(const Vector3<T>& vec) const {
        return RotateVector(vec);
    }
};

template<typename T>
constexpr bool operator==(const Quaternion<T>& a, const Quaternion<T>& b) {
    return a.scalar == b.scalar && a.ratioVec == b.ratioVec;
}

template<typename T>
constexpr bool operator!=(const Quaternion<T>& a, const Quaternion<T>& b) {
    return !(a == b);
}

template<typename T, typename U>
constexpr bool operator==(const Quaternion<T>& a, const Quaternion<U>& b) {
    return a.scalar == b.scalar && a.ratioVec == b.ratioVec;
}

template<typename T, typename U>
constexpr bool operator!=(const Quaternion<T>& a, const Quaternion<U>& b) {
    return !(a == b);
}

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;
using Quaternioni = Quaternion<int>;