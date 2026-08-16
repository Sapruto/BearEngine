#pragma once

#include "Camera.h"
#include "Transform2D.h"
#include "Matrix/Matrix4x4.h"
#include "Vector2.h"

class Camera2D : public Camera {
private:
    Transform2D* transform = nullptr;
    Matrix4x4f viewMatrix;
    Matrix4x4f projMatrix;
    
    float orthoSize = 5.0f;
    float aspect = 1.0f;
    float nearPlane = -1.0f;
    float farPlane = 1.0f;
    
    bool dirty = true;
    
    void UpdateMatrices();
    
public:
    Camera2D() = default;
    explicit Camera2D(float aspect, float orthoSize = 5.0f);
    
    void Start() override;
    void Update() override;
    
    void SetOrthoSize(float size);
    float GetOrthoSize() const { return orthoSize; }
    
    void SetAspect(float aspect);
    float GetAspect() const { return aspect; }
    
    const Matrix4x4f& GetViewMatrix();
    const Matrix4x4f& GetProjectionMatrix();
    Transform2D* GetTransform() { return transform; }
    
    Vector2f ScreenToWorld(const Vector2f& screenPoint, float screenWidth, float screenHeight);
    Vector2f WorldToScreen(const Vector2f& worldPoint, float screenWidth, float screenHeight);
    
    void SetTransform(Transform2D* t) { transform = t; dirty = true; }
};