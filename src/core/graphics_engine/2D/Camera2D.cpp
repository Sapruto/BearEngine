#include "Camera2D.h"
#include <cmath>

namespace {
    const float NEAR_PLANE = -1.0f;
    const float FAR_PLANE = 1.0f;
    const float MIN_ZOOM = 0.1f;
    const float MAX_ZOOM = 10.0f;
}

Camera2D::Camera2D(float width, float height, float orthoSize) 
    :  zoom(1.0f)
    , screenWidth(width)
    , screenHeight(height)
    , isDirty(true)
    , transform(nullptr)
    , orthoSize(orthoSize) {
}

void Camera2D::UpdateMatrices() {
    if (!isDirty || !transform) return;
    
    float aspectRatio = screenWidth / screenHeight;
    float left = -orthoSize * aspectRatio;
    float right = orthoSize * aspectRatio;
    float bottom = -orthoSize;
    float top = orthoSize;
    
    projMatrix = glm::ortho(left, right, bottom, top, NEAR_PLANE, FAR_PLANE);
    
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-transform->position.x, -transform->position.y, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, transform->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    viewMatrix = glm::scale(viewMatrix, glm::vec3(zoom, zoom, 1.0f));
    
    isDirty = false;
}

void Camera2D::SetPosition(const Vector2& pos) {
    if (!transform) return;

    transform->position = pos;
    isDirty = true;
}

void Camera2D::Move(const Vector2& delta) {
    if (!transform) return;

    transform->position += delta;
    isDirty = true;
}

void Camera2D::SetZoom(float z) {
    zoom = z;
    if (zoom < MIN_ZOOM) zoom = MIN_ZOOM;
    if (zoom > MAX_ZOOM) zoom = MAX_ZOOM;
    isDirty = true;
}

void Camera2D::AddZoom(float delta) {
    SetZoom(zoom + delta);
}

const glm::mat4& Camera2D::GetViewMatrix() {
    UpdateMatrices();
    return viewMatrix;
}

const glm::mat4& Camera2D::GetProjectionMatrix() {
    UpdateMatrices();
    return projMatrix;
}

Vector2 Camera2D::ScreenToWorld(const Vector2& screenPoint) {
    UpdateMatrices();
    
    glm::mat4 viewProj = projMatrix * viewMatrix;
    glm::mat4 invViewProj = glm::inverse(viewProj);
    
    float ndcX = (2.0f * screenPoint.x) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPoint.y) / screenHeight;
    
    glm::vec4 worldPoint = invViewProj * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    
    return Vector2(worldPoint.x, worldPoint.y);
}

Vector2 Camera2D::WorldToScreen(const Vector2& worldPoint) {
    UpdateMatrices();
    
    glm::mat4 viewProj = projMatrix * viewMatrix;
    glm::vec4 ndc = viewProj * glm::vec4(worldPoint.x, worldPoint.y, 0.0f, 1.0f);
    
    float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;
    float screenY = (1.0f - ndc.y) * 0.5f * screenHeight;
    
    return Vector2(screenX, screenY);
}

void Camera2D::SetScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    isDirty = true;
}