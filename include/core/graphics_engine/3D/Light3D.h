#pragma once

#include <memory>

#include "Matrix/Matrix4x4.h"
#include "Vector3.h"
#include "Transform3D.h"
#include "Component.h"
#include "SerializeField.h"

class Light3D : public Component {
protected:
    Transform3D* transform{nullptr};

    FIELD(Vector3f, color);   
    FIELD(float, intensity);   

public:
    Light3D() = default;
    Light3D(const Vector3f& col, float intens) {
        color.GetValue() = col;
        intensity.GetValue() = intens;
    }
    virtual ~Light3D() = default;
    
    virtual Matrix4x4f GetLightSpaceMatrix(float aspect = 1.0f) const {
        return Matrix4x4f::Identity();
    }

    void Start() {
        if (!gameObject) return;
        transform = gameObject->GetComponentOfType<Transform3D>();
    }

    Vector3f GetGlobalPosition() const { 
        if (transform) return transform->GetGlobalPosition();
        return Vector3f::Zero();
    }
    Vector3f GetColor() const { return color.GetValue(); }
    float GetIntensity() const { return intensity.GetValue(); }
    
    void SetColor(const Vector3& color) { this->color.GetValue() = color; }
    void SetIntensity(float intensity) { this->intensity.GetValue() = intensity; }

    SERIALIZED_FIELDS(&color, &intensity)
};