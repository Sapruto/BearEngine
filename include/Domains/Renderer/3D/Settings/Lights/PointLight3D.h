#pragma once

#include "Domains/Renderer/3D/Settings/Lights/ILight3D.h"
#include "Math/Vector/Vector3.h"
#include "Math/Transform/Transform3D.h"

class Transform3D;

class PointLight3D : public ILight3D {
private:
    Transform3D* transform{nullptr};

    FIELD(Vector3f, color);
    FIELD(float, intensity);

    FIELD(float, radius);
    FIELD(float, linear);
    FIELD(float, quadratic);

public:
    PointLight3D(float rad = 10.0f) {
        color = Vector3f(1.0f, 1.0f, 1.0f);
        intensity = 1.0f;
        radius = rad;
        linear = 2.0f / rad;
        quadratic = 1.0f / (rad * rad);
    }

    void Start() override {
        if (!gameObject) return;
        transform = gameObject->GetComponentOfType<Transform3D>();
    }

    Vector3f GetGlobalPosition() const {
        return transform ? transform->GetGlobalPosition() : Vector3f::Zero();
    }

    Vector3f GetColor() const { return color.GetValue(); }
    float GetIntensity() const { return intensity.GetValue(); }

    void SetColor(const Vector3f& c) { color = c; }
    void SetIntensity(float i) { intensity = i; }

    float GetRadius() const { return radius.GetValue(); }
    void SetRadius(float rad) {
        radius = rad;
        linear = 2.0f / rad;
        quadratic = 1.0f / (rad * rad);
    }

    float GetLinear() const { return linear.GetValue(); }
    float GetQuadratic() const { return quadratic.GetValue(); }

    LightData GetLightData() const override;

    SERIALIZED_FIELDS(&color, &intensity, &radius, &linear, &quadratic)
};