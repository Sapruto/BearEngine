#include "Camera2D.h"
#include <cmath>

namespace {
    const float NEAR_PLANE = -1.0f;
    const float FAR_PLANE = 1.0f;
    const float MIN_ZOOM = 0.1f;
    const float MAX_ZOOM = 10.0f;
}

Camera2D::Camera2D(float width, float height) 
    : position(0.0f, 0.0f)
    , rotation(0.0f)
    , zoom(1.0f)
    , screenWidth(width)
    , screenHeight(height)
    , needsUpdate(true) {
}

void Camera2D::UpdateMatrices() {
    if (!needsUpdate) return;
    
    float aspectRatio = screenWidth / screenHeight;
    float orthoSize = 5.0f;
    float left = -orthoSize * aspectRatio;
    float right = orthoSize * aspectRatio;
    float bottom = -orthoSize;
    float top = orthoSize;
    
    projMatrix = glm::ortho(left, right, bottom, top, NEAR_PLANE, FAR_PLANE);
    
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-position, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    viewMatrix = glm::scale(viewMatrix, glm::vec3(zoom, zoom, 1.0f));
    
    needsUpdate = false;
}

void Camera2D::SetPosition(const glm::vec2& pos) {
    position = pos;
    needsUpdate = true;
}

void Camera2D::Move(const glm::vec2& delta) {
    position += delta;
    needsUpdate = true;
}

void Camera2D::SetZoom(float z) {
    zoom = z;
    if (zoom < MIN_ZOOM) zoom = MIN_ZOOM;
    if (zoom > MAX_ZOOM) zoom = MAX_ZOOM;
    needsUpdate = true;
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

glm::vec2 Camera2D::ScreenToWorld(const glm::vec2& screenPoint) {
    UpdateMatrices();
    
    glm::mat4 viewProj = projMatrix * viewMatrix;
    glm::mat4 invViewProj = glm::inverse(viewProj);
    
    float ndcX = (2.0f * screenPoint.x) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPoint.y) / screenHeight;
    
    glm::vec4 worldPoint = invViewProj * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    
    return glm::vec2(worldPoint.x, worldPoint.y);
}

glm::vec2 Camera2D::WorldToScreen(const glm::vec2& worldPoint) {
    UpdateMatrices();
    
    glm::mat4 viewProj = projMatrix * viewMatrix;
    glm::vec4 ndc = viewProj * glm::vec4(worldPoint, 0.0f, 1.0f);
    
    float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;
    float screenY = (1.0f - ndc.y) * 0.5f * screenHeight;
    
    return glm::vec2(screenX, screenY);
}

void Camera2D::SetScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    needsUpdate = true;
}