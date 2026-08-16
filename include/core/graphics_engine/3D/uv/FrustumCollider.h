#pragma once

#include "Polyhedron3D.h"
#include "Camera3D.h"
#include "Matrix/Matrix4x4.h"

class FrustumCollider : public Polyhedron3D {
private:
    Camera3D* camera = nullptr;
    float aspect = 1.0f;
    float fov = 60.0f;
    float near = 0.1f;
    float far = 100.0f;
    
    Matrix4x4f lastViewProj;
    bool dirty = true;
    
    void UpdateFrustum();
    void BuildFrustumPolyhedron(const Matrix4x4f& viewProj);
    Vector3f UnprojectPoint(const Vector3f& ndc, const Matrix4x4f& invViewProj);
    
public:
    FrustumCollider() = default;
    explicit FrustumCollider(Camera3D* camera, float aspect, float fov = 60.0f, float near = 0.1f, float far = 100.0f);
    
    void SetCamera(Camera3D* camera) { this->camera = camera; dirty = true; }
    void SetAspect(float aspect) { this->aspect = aspect; dirty = true; }
    void SetFOV(float fov) { this->fov = fov; dirty = true; }
    void SetNearFar(float near, float far) { this->near = near; this->far = far; dirty = true; }
    
    void Update() override;
    
    bool IsVisible(const Polyhedron3D* object) const;
    bool IsVisible(const AABB& aabb) const;
    bool IsVisible(const Vector3f& point) const;
};