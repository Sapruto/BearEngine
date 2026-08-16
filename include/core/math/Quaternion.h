#pragma once

#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include "Vector3.h"

template<typename T = float>
struct Quaternion {
    Vector3Impl<T> ratioVec;
    T scalar;

    Quaternion() : ratioVec(Vector3Impl<T>::Zero()), scalar(T(0)) {}
    constexpr Quaternion(Vector3Impl<T> ratioVec, T scalar) 
        : ratioVec(ratioVec), scalar(scalar) {}

    static Quaternion Identity() {
        return Quaternion(Vector3Impl<T>(0, 0, 0), T(1));
    }
    
    static Quaternion createRotation(const T& angle, const Vector3Impl<T>& axis) {
        T halfA = angle / T(2);
        T sinA = std::sin(halfA);
        T cosA = std::cos(halfA);

        Vector3Impl<T> normalizedAxis = axis.normalized();
        
        Quaternion q;
        q.ratioVec = normalizedAxis * sinA;
        q.scalar = cosA;
        
        return q;
    }
    
    static Quaternion slerp(const Quaternion& a, const Quaternion& b, T t) {
        T dot = a.scalar * b.scalar + a.ratioVec.dot(b.ratioVec);
        Quaternion b2 = b;
        
        if (dot < T(0)) {
            b2 = -b;
            dot = -dot;
        }
        
        if (dot > T(0.9995)) {
            Quaternion result = a + (b2 - a) * t;
            return result.normalized();
        }
        
        T theta = std::acos(dot);
        T sinTheta = std::sin(theta);
        T w1 = std::sin((T(1) - t) * theta) / sinTheta;
        T w2 = std::sin(t * theta) / sinTheta;
        
        return a * w1 + b2 * w2;
    }

    Quaternion conjugate() const {
        return Quaternion(Vector3Impl<T>(-ratioVec.x, -ratioVec.y, -ratioVec.z), scalar);
    }

    T magnitude() const {
        return std::sqrt(scalar * scalar 
                        + ratioVec.x * ratioVec.x 
                        + ratioVec.y * ratioVec.y 
                        + ratioVec.z * ratioVec.z);
    }

    Quaternion normalized() const {
        T mag = magnitude();
        if (mag == T(0)) return Identity();
        return Quaternion(Vector3Impl<T>(ratioVec.x / mag, 
                                        ratioVec.y / mag, 
                                        ratioVec.z / mag), 
                            scalar / mag);
    }

    Quaternion inverse() const {
        T normSq = scalar * scalar 
                    + ratioVec.x * ratioVec.x 
                    + ratioVec.y * ratioVec.y 
                    + ratioVec.z * ratioVec.z;
        
        if (normSq == T(0)) return Identity();
        
        Quaternion conj = conjugate();
        return Quaternion(Vector3Impl<T>(conj.ratioVec.x / normSq,
                                        conj.ratioVec.y / normSq,
                                        conj.ratioVec.z / normSq),
                            conj.scalar / normSq);
    }

    Vector3Impl<T> rotateVector(const Vector3Impl<T>& vec) const {
        Vector3Impl<T> vecPart = ratioVec;
        T scalarPart = scalar;
        
        Vector3Impl<T> t = vecPart.cross(vec) * T(2);
        return vec + t * scalarPart + vecPart.cross(t);
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
        return Quaternion(Vector3Impl<T>(ratioVec.x + other.ratioVec.x,
                                        ratioVec.y + other.ratioVec.y,
                                        ratioVec.z + other.ratioVec.z),
                            scalar + other.scalar);
    }
    
    Quaternion operator-(const Quaternion& other) const {
        return Quaternion(Vector3Impl<T>(ratioVec.x - other.ratioVec.x,
                                        ratioVec.y - other.ratioVec.y,
                                        ratioVec.z - other.ratioVec.z),
                            scalar - other.scalar);
    }
    
    Quaternion operator*(const T& scale) const {
        return Quaternion(Vector3Impl<T>(ratioVec.x * scale,
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
    
    static std::string ToString(const Quaternion& q) {
        return std::to_string(q.ratioVec.x) + "," +
               std::to_string(q.ratioVec.y) + "," +
               std::to_string(q.ratioVec.z) + "," +
               std::to_string(q.scalar);
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

    Vector3Impl<T> operator*(const Vector3Impl<T>& vec) const {
        return rotateVector(vec);
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