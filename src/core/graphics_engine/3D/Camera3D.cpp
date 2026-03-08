#pragma once

#include "Camera3D.h"

Camera3D::Camera3D(Vector3 pos) 
    : position(pos), worldUp(Vector3(0, 1, 0)) {
    updateVectors();
}

glm::mat4 Camera3D::GetViewMatrix() {
    return glm::lookAt(
        glm::vec3(position.x, position.y, position.z),
        glm::vec3(position.x + front.x, position.y + front.y, position.z + front.z),
        glm::vec3(up.x, up.y, up.z)
    );
}

glm::mat4 Camera3D::GetProjectionMatrix(float aspectRatio) {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera3D::updateVectors() {
    Vector3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = newFront.normalized();
    
    right = front.cross(worldUp).normalized();
    up = right.cross(front).normalized();
}

void Camera3D::ProcessMouse(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    updateVectors();
}