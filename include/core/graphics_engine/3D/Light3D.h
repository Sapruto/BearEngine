#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Vector3.h"

class Light3D {
protected:
    Vector3 position;

    Vector3 color;   
    float intensity;   

public:
    Light3D() = default;
    Light3D(const Vector3& col, float intens) 
        : color(col), intensity(intens) {}
    virtual ~Light3D() = default;
    
    virtual glm::mat4 GetLightSpaceMatrix(float aspect = 1.0f) const {
        return glm::mat4(1.0f);
    }

    Vector3 GetPosition() const { return position; }
    Vector3 GetColor() const { return color; }
    float GetIntensity() const { return intensity; }
    
    void SetPosition(const Vector3& position) { this->position = position; }
    void SetColor(const Vector3& color) { this->color = color; }
    void SetIntensity(float intensity) { this->intensity = intensity; }
};