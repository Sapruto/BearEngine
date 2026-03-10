#pragma once

#include "Light3D.h"

class PointLight3D : public Light3D{
private:
    float radius; 
    float attenuationCoefficient; 
    float linear;
    float quadratic;

public:    
    PointLight3D(const Vector3& pos, const Vector3& col, float intens, float rad = 10.0f) 
        : radius(rad)
    {
        position = pos;
        color = col;
        intensity = intens;
        
        attenuationCoefficient = 1.0f;
        linear = 2.0f / rad;
        quadratic = 1.0f / (rad * rad);
    }
    
    float GetRadius() const { return radius; }
    void SetRadius(float rad) {
        radius = rad;
        linear = 2.0f / rad;
        quadratic = 1.0f / (rad * rad);
    }
    
    float GetAttenuationCoefficient() const { return attenuationCoefficient; }
    float GetLinear() const { return linear; }
    float GetQuadratic() const { return quadratic; }

    glm::mat4 GetLightSpaceMatrix(float aspect = 1.0f) const override;
};