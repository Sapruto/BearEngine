#include "Domains/Renderer/3D/Settings/Cameras/Camera3D.h"

#include "Math/Transform/Transform3D.h"
#include "Math/Quaternion.h"

void Camera3D::Start() {
    if (gameObject) {
        transform = gameObject->GetComponentOfType<Transform3D>();
    }
}

Vector3f Camera3D::GetGlobalPosition() const {
    return transform ? transform->GetGlobalPosition() : Vector3f::Zero();
}

Matrix4x4f Camera3D::GetViewMatrix() const {
    if (!transform) {
        return Matrix4x4f::Identity();
    }
    
    const Vector3f& pos = transform->GetGlobalPosition();
    const Quaternionf& rot = transform->GetGlobalRotation();
    
    Vector3f front = rot.RotateVector(Vector3f(0, 0, -1));
    Vector3f up = rot.RotateVector(Vector3f(0, 1, 0));
    
    Vector3f target = pos + front;
    
    return Matrix4x4f::LookAt(pos, target, up);
}

Matrix4x4f Camera3D::GetProjectionMatrix(float aspectRatio) const {
    return Matrix4x4f::Perspective(45.0f, aspectRatio, 0.1f, 100.0f);
}