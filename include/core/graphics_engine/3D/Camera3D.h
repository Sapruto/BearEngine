#pragma once

#include "Camera.h"

#include "Vector3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera3D : public Camera {
public:
    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;
    Vector3 worldUp;
    
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;
    
public:
    Camera3D(Vector3 pos = Vector3(0, 0, 5));
    
    glm::mat4 GetViewMatrix();
    
    glm::mat4 GetProjectionMatrix(float aspectRatio);
    
    void updateVectors();

    void ProcessMouse(float xoffset, float yoffset);
};