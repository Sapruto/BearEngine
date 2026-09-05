#pragma once

#include "Light3D.h"
#include "SerializeField.h"
#include "Matrix/Matrix4x4.h"
#include "Vector3.h"

class PointLight3D : public Light3D {
private:
    FIELD(float, radius);
    FIELD(float, linear);
    FIELD(float, quadratic);

public:
    PointLight3D() {
        radius.GetValue() = 10.0f;
        linear.GetValue() = 2.0f / 10.0f;
        quadratic.GetValue() = 1.0f / (10.0f * 10.0f);
    }
    
    PointLight3D(const Vector3f& col, float intens, float rad = 10.0f)
        : Light3D(col, intens) {
        radius.GetValue() = rad;
        linear.GetValue() = 2.0f / rad;
        quadratic.GetValue() = 1.0f / (rad * rad);
    }
    
    float GetRadius() const { return radius.GetValue(); }
    void SetRadius(float rad) {
        radius.GetValue() = rad;
        linear.GetValue() = 2.0f / rad;
        quadratic.GetValue() = 1.0f / (rad * rad);
    }
    
    float GetLinear() const { return linear.GetValue(); }
    float GetQuadratic() const { return quadratic.GetValue(); }
    
    Matrix4x4f GetLightSpaceMatrix(float aspect = 1.0f) const override {
        return Matrix4x4f::Identity();
    }

    SERIALIZED_FIELDS(&radius, &linear, &quadratic)
};