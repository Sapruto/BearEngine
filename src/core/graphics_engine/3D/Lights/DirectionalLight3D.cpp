#include "DirectionalLight3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

DirectionalLight3D::DirectionalLight3D(
    const Vector3& dir,
    const Vector3& col,
    float intens
) : Light3D(col, intens),
    direction(dir)
{
    direction.normalize();
}

Vector3 DirectionalLight3D::GetDirection() const { 
    return direction; 
}

glm::mat4 DirectionalLight3D::GetLightSpaceMatrix(float aspect) const {
    glm::mat4 lightProjection = glm::ortho(
        -shadowAreaSize, shadowAreaSize,
        -shadowAreaSize, shadowAreaSize,
        nearPlane, farPlane      
    );
    
    glm::vec3 dir(direction.x, direction.y, direction.z);
    glm::vec3 lightPos = -dir * DEFAULT_LIGHT_OFFSET;
    glm::vec3 lightTarget = lightPos + dir;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    if (std::abs(dir.y) > DIRECTION_UP_THRESHOLD) {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, up);
    
    return lightProjection * lightView;
}

void DirectionalLight3D::SetDirection(const Vector3& direction) { 
    this->direction = direction;
    this->direction.normalize();
}

void DirectionalLight3D::SetShadowArea(float size) { 
    shadowAreaSize = size; 
}

void DirectionalLight3D::SetShadowPlanes(float nearPlane, float farPlane) { 
    this->nearPlane = nearPlane; 
    this->farPlane = farPlane; 
}