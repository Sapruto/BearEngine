#include "Camera2D.h"

#include "Transform2D.h"
#include <cmath>

Camera2D::Camera2D(float aspect, float orthoSize)
    : aspect(aspect), orthoSize(orthoSize), dirty(true) {}

void Camera2D::Start() {
    if (gameObject) {
        transform = gameObject->GetComponentOfType<Transform2D>();
        if (!transform) {
            transform = gameObject->AddComponent<Transform2D>();
        }
        dirty = true;
    }
}

void Camera2D::Update() {
    if (dirty) UpdateMatrices();
}

void Camera2D::UpdateMatrices() {
    if (!transform) return;
    
    float halfWidth = orthoSize * aspect;
    float halfHeight = orthoSize;
    
    projMatrix = Matrix4x4f::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        nearPlane, farPlane
    );
    
    Vector2f pos = transform->GetGlobalPosition();
    float rot = transform->GetAngle();
    Vector2f scale = transform->GetGlobalScale();
    
    Matrix4x4f translateMatrix = Matrix4x4f::Identity();
    translateMatrix(0, 3) = -pos.x;
    translateMatrix(1, 3) = -pos.y;
    
    Matrix4x4f rotateMatrix = Matrix4x4f::Identity();
    float cosA = cosf(-rot);
    float sinA = sinf(-rot);
    rotateMatrix(0, 0) = cosA;
    rotateMatrix(0, 1) = -sinA;
    rotateMatrix(1, 0) = sinA;
    rotateMatrix(1, 1) = cosA;
    
    Matrix4x4f scaleMatrix = Matrix4x4f::Identity();
    scaleMatrix(0, 0) = 1.0f / scale.x;
    scaleMatrix(1, 1) = 1.0f / scale.y;
    
    viewMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    
    dirty = false;
}

void Camera2D::SetOrthoSize(float size) {
    orthoSize = size;
    dirty = true;
}

void Camera2D::SetAspect(float a) {
    aspect = a;
    dirty = true;
}

const Matrix4x4f& Camera2D::GetViewMatrix() {
    if (dirty) UpdateMatrices();
    return viewMatrix;
}

const Matrix4x4f& Camera2D::GetProjectionMatrix() {
    if (dirty) UpdateMatrices();
    return projMatrix;
}

Vector2f Camera2D::ScreenToWorld(const Vector2f& screenPoint, float screenWidth, float screenHeight) {
    if (dirty) UpdateMatrices();
    
    Matrix4x4f viewProj = projMatrix * viewMatrix;
    Matrix4x4f invViewProj = Matrix4x4f(viewProj.inverse());
    
    float ndcX = (2.0f * screenPoint.x) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPoint.y) / screenHeight;
    
    Vector3f worldPos = invViewProj * Vector3f(ndcX, ndcY, 0.0f);
    return Vector2f(worldPos.x, worldPos.y);
}

Vector2f Camera2D::WorldToScreen(const Vector2f& worldPoint, float screenWidth, float screenHeight) {
    if (dirty) UpdateMatrices();
    
    Matrix4x4f viewProj = projMatrix * viewMatrix;
    Vector3f ndc = viewProj * Vector3f(worldPoint.x, worldPoint.y, 0.0f);
    
    float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;
    float screenY = (1.0f - ndc.y) * 0.5f * screenHeight;
    
    return Vector2f(screenX, screenY);
}