#include "Vector3.h"

float Vector3::magnitude() const {
    return sqrtf(x * x + y * y + z * z);
}

float Vector3::sqrMagnitude() const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::normalized() const {
    float mag = magnitude();
    if (mag > 0.00001f) {
        return Vector3(x / mag, y / mag, z / mag);
    }
    return Vector3(0, 0, 0);
}

void Vector3::normalize() {
    float mag = magnitude();
    if (mag > 0.00001f) {
        x /= mag;
        y /= mag;
        z /= mag;
    }
}

float Vector3::distanceTo(const Vector3& other) const {
    return (*this - other).magnitude();
}

float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

float Vector3::angle(const Vector3& from, const Vector3& to) {
    float dot = from.dot(to);
    float mags = from.magnitude() * to.magnitude();
    if (mags < 0.00001f) return 0;
    return acosf(dot / mags) * 180.0f / 3.14159265f;
}

Vector3 Vector3::clampMagnitude(float maxLength) const {
    float mag = magnitude();
    if (mag > maxLength) {
        return normalized() * maxLength;
    }
    return *this;
}

Vector3 Vector3::reflect(const Vector3& normal) const {
    return *this - normal * (2.0f * this->dot(normal));
}

Vector3 Vector3::lerp(const Vector3& a, const Vector3& b, float t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    return a + (b - a) * t;
}

Vector3 Vector3::moveTowards(const Vector3& current, const Vector3& target, 
                            float maxDistanceDelta) {
    Vector3 toTarget = target - current;
    float dist = toTarget.magnitude();
    
    if (dist <= maxDistanceDelta || dist == 0) {
        return target;
    }
    
    return current + toTarget / dist * maxDistanceDelta;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

const Vector3 Vector3::Zero(0, 0, 0);
const Vector3 Vector3::One(1, 1, 1);
const Vector3 Vector3::Forward(0, 0, 1);
const Vector3 Vector3::Back(0, 0, -1);
const Vector3 Vector3::Up(0, 1, 0);
const Vector3 Vector3::Down(0, -1, 0);
const Vector3 Vector3::Right(1, 0, 0);
const Vector3 Vector3::Left(-1, 0, 0);