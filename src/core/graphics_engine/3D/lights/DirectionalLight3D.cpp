#include "DirectionalLight3D.h"

#include "Matrix/Matrix4x4.h"
#include <cmath>

DirectionalLight3D::DirectionalLight3D()
    : Light3D() {
    direction.GetValue() = Vector3f(0.0f, -1.0f, 0.0f);
    shadowAreaSize.GetValue() = 50.0f;
    nearPlane.GetValue() = 1.0f;
    farPlane.GetValue() = 100.0f;
}

DirectionalLight3D::DirectionalLight3D(const Vector3f& dir, const Vector3f& col, float intens)
    : Light3D(col, intens) {
    direction.GetValue() = dir.normalized();
    shadowAreaSize.GetValue() = 50.0f;
    nearPlane.GetValue() = 1.0f;
    farPlane.GetValue() = 100.0f;
}

void DirectionalLight3D::SetDirection(const Vector3f& dir) {
    direction.GetValue() = dir.normalized();
}

Matrix4x4f DirectionalLight3D::GetLightSpaceMatrix(float aspect) const {
    Matrix4x4f lightProjection = Matrix4x4f::ortho(
        -shadowAreaSize.GetValue(), shadowAreaSize.GetValue(),
        -shadowAreaSize.GetValue(), shadowAreaSize.GetValue(),
        nearPlane.GetValue(), farPlane.GetValue()
    );
    
    Vector3f dir = direction.GetValue().normalized();
    Vector3f lightPos = -dir * DEFAULT_LIGHT_OFFSET;
    Vector3f lightTarget = lightPos + dir;
    Vector3f up(0.0f, 1.0f, 0.0f);
    
    if (std::abs(dir.y) > DIRECTION_UP_THRESHOLD) {
        up = Vector3f(0.0f, 0.0f, 1.0f);
    }
    
    Matrix4x4f lightView = Matrix4x4f::lookAt(lightPos, lightTarget, up);
    
    return Matrix4x4f((lightProjection * lightView).GetData());
}