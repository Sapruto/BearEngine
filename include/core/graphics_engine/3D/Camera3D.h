#pragma once

#include "Camera.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"

class Transform3D;

class Camera3D : public Camera {
private:
    const Transform3D* transform{nullptr};
    
public:
    Camera3D() = default;
    
    void Start() override;

    const Transform3D* GetTarget() const { return transform; }
    
    Vector3f GetPosition() const;
    
    Matrix4x4f GetViewMatrix();
    Matrix4x4f GetProjectionMatrix(float aspectRatio);
};