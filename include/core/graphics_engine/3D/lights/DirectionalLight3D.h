#pragma once

#include "Light3D.h"
#include "SerializeField.h"
#include "Matrix/Matrix4x4.h"
#include "Vector3.h"

class DirectionalLight3D : public Light3D {
private:
    FIELD(Vector3f, direction);
    FIELD(float, shadowAreaSize);
    FIELD(float, nearPlane);
    FIELD(float, farPlane);

    static constexpr float DEFAULT_LIGHT_OFFSET = 50.0f;
    static constexpr float DIRECTION_UP_THRESHOLD = 0.99f;

public:
    DirectionalLight3D();
    DirectionalLight3D(const Vector3f& dir, const Vector3f& col, float intens);
    
    Vector3f GetDirection() const { return direction.GetValue(); }
    void SetDirection(const Vector3f& dir);
    
    float GetShadowArea() const { return shadowAreaSize.GetValue(); }
    void SetShadowArea(float size) { shadowAreaSize.GetValue() = size; }
    
    float GetNearPlane() const { return nearPlane.GetValue(); }
    float GetFarPlane() const { return farPlane.GetValue(); }
    void SetShadowPlanes(float nearP, float farP) {
        nearPlane.GetValue() = nearP;
        farPlane.GetValue() = farP;
    }
    
    Matrix4x4f GetLightSpaceMatrix(float aspect = 1.0f) const override;

    SERIALIZED_FIELDS(&direction, &shadowAreaSize, &nearPlane, &farPlane)
};