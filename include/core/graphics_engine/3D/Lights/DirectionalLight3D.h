#pragma once

#include "Light3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vector3.h"

class DirectionalLight3D : public Light3D {
private:
    Vector3 direction;

    float shadowAreaSize = 50.0f; 
    float nearPlane = 1.0f;
    float farPlane = 100.0f;

    static constexpr float DEFAULT_POSITION_SCALE = 1000.0f;
    static constexpr float DEFAULT_LIGHT_OFFSET = 50.0f;
    static constexpr float DIRECTION_UP_THRESHOLD = 0.99f;

public:
    DirectionalLight3D(const Vector3& direction,
    const Vector3& color,
    float intensity);
    
    glm::mat4 GetLightSpaceMatrix(float aspect = 1.0f) const override;

    Vector3 GetDirection() const;
    
    void SetDirection(const Vector3& direction);
    void SetShadowArea(float size);
    void SetShadowPlanes(float nearPlane, float farPlane);
};