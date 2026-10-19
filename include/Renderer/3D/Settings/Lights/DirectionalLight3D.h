#pragma once

#include "Renderer/3D/Settings/Lights/ILight3D.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector3.h"

class DirectionalLight3D : public ILight3D {
private:
    FIELD(Vector3f, direction);
    FIELD(Vector3f, color);
    FIELD(float, intensity);
    FIELD(float, shadowAreaSize);
    FIELD(float, nearPlane);
    FIELD(float, farPlane);

    static constexpr float DEFAULT_LIGHT_OFFSET = 50.0f;
    static constexpr float DIRECTION_UP_THRESHOLD = 0.99f;

public:
    DirectionalLight3D();
    DirectionalLight3D(const Vector3f& dir, const Vector3f& col, float intens);
    
    void SetColor(const Vector3f& col);
    void SetDirection(const Vector3f& dir);
    void SetIntensity(float intens) { intensity.GetValue() = intens; }
    
    float GetShadowArea() const { return shadowAreaSize.GetValue(); }
    void SetShadowArea(float size) { shadowAreaSize.GetValue() = size; }
    
    Vector3f GetDirection() const { return direction.GetValue(); }
    Vector3f GetColor() const { return color.GetValue(); }
    float GetNearPlane() const { return nearPlane.GetValue(); }
    float GetFarPlane() const { return farPlane.GetValue(); }
    float GetIntensity() const { return intensity.GetValue(); }
    void SetShadowPlanes(float nearP, float farP) {
        nearPlane.GetValue() = nearP;
        farPlane.GetValue() = farP;
    }
    
    Matrix4x4f GetLightSpaceMatrix() const;
    LightData GetLightData() const override;

    SERIALIZED_FIELDS(&direction, &color, &intensity, &shadowAreaSize, &nearPlane, &farPlane)
};