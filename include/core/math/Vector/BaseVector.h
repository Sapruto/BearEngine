#pragma once

#include <cmath>
#include <string>
#include <sstream>
#include <cstddef>
#include <concepts>

template <typename VectorImpl, typename T, int Count>
struct BaseVector {
private:
    constexpr void CheckOperator() const {
        static_assert(
            requires(const VectorImpl& v, int i) {
                { v[i] } -> std::convertible_to<const T&>;
            },
            "VectorImpl must provide operator[](int) -> const T&"
        );
    }

public:
    constexpr BaseVector() { CheckOperator(); }

    constexpr VectorImpl& vectorImpl() { return static_cast<VectorImpl&>(*this); }
    constexpr const VectorImpl& vectorImpl() const { return static_cast<const VectorImpl&>(*this); }

    constexpr T sqrMagnitude() const {
        T sum = T();
        for (int i = 0; i < Count; i++) sum += vectorImpl()[i] * vectorImpl()[i];
        return sum;
    }

    T magnitude() const {
        return std::sqrt(sqrMagnitude());
    }

    void normalize() {
        T mag = magnitude();
        if (mag > static_cast<T>(1e-5)) {
            for (int i = 0; i < Count; i++) vectorImpl()[i] /= mag;
        }
    }

    VectorImpl normalized() const {
        T mag = magnitude();
        if (mag > static_cast<T>(1e-5)) {
            VectorImpl r;
            for (int i = 0; i < Count; i++) r[i] = vectorImpl()[i] / mag;
            return r;
        }
        return VectorImpl();
    }

    constexpr T dot(const VectorImpl& other) const {
        T sum = T();
        for (int i = 0; i < Count; i++) sum += vectorImpl()[i] * other[i];
        return sum;
    }

    T distanceTo(const VectorImpl& other) const {
        return (vectorImpl() - other).magnitude();
    }

    VectorImpl clampMagnitude(T maxLength) const {
        T mag = magnitude();
        if (mag > maxLength) return normalized() * maxLength;
        return vectorImpl();
    }

    VectorImpl reflect(const VectorImpl& normal) const {
        return vectorImpl() - normal * (static_cast<T>(2.0) * dot(normal));
    }

    static constexpr VectorImpl Zero() {
        VectorImpl r;
        for (int i = 0; i < Count; i++) r[i] = T(0);
        return r;
    }

    static constexpr VectorImpl One() {
        VectorImpl r;
        for (int i = 0; i < Count; i++) r[i] = T(1);
        return r;
    }

    static constexpr VectorImpl lerp(const VectorImpl& a, const VectorImpl& b, T t) {
        t = t < 0 ? 0 : t > 1 ? 1 : t;
        return a + (b - a) * t;
    }

    static VectorImpl moveTowards(const VectorImpl& current, const VectorImpl& target, T maxDistanceDelta) {
        VectorImpl toTarget = target - current;
        T dist = toTarget.magnitude();
        if (dist <= maxDistanceDelta || dist == 0) return target;
        return current + toTarget / dist * maxDistanceDelta;
    }

    static std::string ToString(const VectorImpl& v) {
        std::ostringstream oss;
        oss << "(";
        for (int i = 0; i < Count; i++) {
            if (i > 0) oss << ", ";
            oss << v[i];
        }
        oss << ")";
        return oss.str();
    }

    static VectorImpl FromString(const std::string& str) {
        VectorImpl v;
        std::istringstream iss(str);
        char open, comma, close;
        if (!(iss >> open) || open != '(') return VectorImpl();
        for (int i = 0; i < Count; i++) {
            if (i > 0) {
                if (!(iss >> comma) || comma != ',') return VectorImpl();
            }
            if (!(iss >> v[i])) return VectorImpl();
        }
        if (!(iss >> close) || close != ')') return VectorImpl();
        return v;
    }

    constexpr VectorImpl& operator+=(const VectorImpl& o) {
        for (int i = 0; i < Count; i++) vectorImpl()[i] += o[i];
        return vectorImpl();
    }
    constexpr VectorImpl& operator-=(const VectorImpl& o) {
        for (int i = 0; i < Count; i++) vectorImpl()[i] -= o[i];
        return vectorImpl();
    }
    constexpr VectorImpl& operator*=(T s) {
        for (int i = 0; i < Count; i++) vectorImpl()[i] *= s;
        return vectorImpl();
    }
    constexpr VectorImpl& operator/=(T s) {
        for (int i = 0; i < Count; i++) vectorImpl()[i] /= s;
        return vectorImpl();
    }

    friend constexpr bool operator==(const VectorImpl& a, const VectorImpl& b) {
        for (int i = 0; i < Count; i++) if (a[i] != b[i]) return false;
        return true;
    }
    friend constexpr bool operator!=(const VectorImpl& a, const VectorImpl& b) { return !(a == b); }

    friend constexpr VectorImpl operator+(const VectorImpl& a, const VectorImpl& b) {
        VectorImpl r = a; r += b; return r;
    }
    friend constexpr VectorImpl operator-(const VectorImpl& a, const VectorImpl& b) {
        VectorImpl r = a; r -= b; return r;
    }
    friend constexpr VectorImpl operator*(const VectorImpl& v, T s) {
        VectorImpl r = v; r *= s; return r;
    }
    friend constexpr VectorImpl operator*(T s, const VectorImpl& v) { return v * s; }
    friend constexpr VectorImpl operator/(const VectorImpl& v, T s) {
        VectorImpl r = v; r /= s; return r;
    }
    constexpr VectorImpl operator-() const {
        VectorImpl r;
        for (int i = 0; i < Count; i++) r[i] = -vectorImpl()[i];
        return r;
    }
};

template <typename Derived, typename T, int Count>
struct BaseVectorAlias : BaseVector<Derived, T, Count> {
    using Base = BaseVector<Derived, T, Count>;

    using Base::Zero;
    using Base::One;
    using Base::lerp;
    using Base::moveTowards;
    using Base::ToString;
    using Base::FromString;
};