#include "Transform3D.h"

#include <cmath>

Transform3D::Transform3D() 
    : position(0, 0, 0)
    , scale(1, 1, 1)
    , rotation(0, 0, 0)
    , fieldPosition("fieldPosition", Vector3(0, 0, 0))
    , fieldScale("fieldScale", Vector3(1, 1, 1))
    , fieldRotation("fieldRotation", Vector3(0, 0, 0))
{
}

void Transform3D::rotate(float pitch, float yaw, float roll) {
    rotation.x += pitch;
    rotation.y += yaw;
    rotation.z += roll;
    normalizeAngles();
}

void Transform3D::normalizeAngles() {
    const float twoPI = 6.283185307f;
    const float PI = 3.1415926535f;
    
    rotation.x = fmodf(rotation.x + PI, twoPI);
    if (rotation.x < 0) rotation.x += twoPI;
    rotation.x -= PI;
    
    rotation.y = fmodf(rotation.y + PI, twoPI);
    if (rotation.y < 0) rotation.y += twoPI;
    rotation.y -= PI;
    
    rotation.z = fmodf(rotation.z + PI, twoPI);
    if (rotation.z < 0) rotation.z += twoPI;
    rotation.z -= PI;
}

Vector3 Transform3D::forward() const {
    float cosYaw = cosf(rotation.y);
    float sinYaw = sinf(rotation.y);
    float cosPitch = cosf(rotation.x);
    float sinPitch = sinf(rotation.x);
    
    return Vector3(
        cosYaw * cosPitch,
        sinYaw * cosPitch,
        sinPitch
    ).normalized();
}

Vector3 Transform3D::backward() const {
    return -forward();
}

Vector3 Transform3D::right() const {
    float cosYaw = cosf(rotation.y);
    float sinYaw = sinf(rotation.y);
    float cosRoll = cosf(rotation.z);
    float sinRoll = sinf(rotation.z);
    
    Vector3 right(
        cosYaw,
        sinYaw,
        0
    );
    
    if (fabsf(rotation.z) > 0.001f) {
        Vector3 fwd = forward();
        right = right * cosRoll + up() * sinRoll;
    }
    
    return right.normalized();
}

Vector3 Transform3D::left() const {
    return -right();
}

Vector3 Transform3D::up() const {
    Vector3 fwd = forward();
    Vector3 rgt = right();
    return Vector3(
        rgt.y * fwd.z - rgt.z * fwd.y,
        rgt.z * fwd.x - rgt.x * fwd.z,
        rgt.x * fwd.y - rgt.y * fwd.x
    ).normalized();
}

Vector3 Transform3D::down() const {
    return -up();
}

void Transform3D::lookAt(const Vector3& target) {
    Vector3 direction = (target - position).normalized();
    
    rotation.y = atan2f(direction.y, direction.x);
    
    float horizontalDistance = sqrtf(direction.x * direction.x + direction.y * direction.y);
    rotation.x = atan2f(-direction.z, horizontalDistance);
    
    rotation.z = 0;
}