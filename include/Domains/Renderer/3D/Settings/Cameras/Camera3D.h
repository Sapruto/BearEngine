#pragma once

#include "../../../Base/Camera.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector3.h"

class Transform3D;

class Camera3D : public Camera {
private:
    const Transform3D* transform{nullptr};
    
public:
    Camera3D() = default;
    
    void Start() override;

    const Transform3D* GetTarget() const { return transform; }
    
    Vector3f GetGlobalPosition() const;
    
    Matrix4x4f GetViewMatrix();
    Matrix4x4f GetProjectionMatrix(float aspectRatio);
};