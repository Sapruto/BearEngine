#include "Math/Transform/Transform3D.h"

#include "World/GameObject.h"
#include "World/Hierarchy/HierarchySystem.h"
#include <cmath>

Transform3D::Transform3D()
    : fieldPosition("fieldPosition", Vector3f::Zero())
    , fieldScale("fieldScale", Vector3f::One())
    , fieldRotation("fieldRotation", Quaternionf::Identity())
    , isDirty(true)
{
    cachedMatrix = Matrix4x4f::Identity();
}

void Transform3D::UpdateWorld() {
    if (!isDirty) return;

    Vector3f gp = fieldPosition.GetValue();
    Vector3f gs = fieldScale.GetValue();
    Quaternionf gr = fieldRotation.GetValue();

    HierarchySystem* sys = gameObject->GetHierarchySystem();
    if (sys) {
        std::vector<GameObject*> ps = sys->GetParents(gameObject);
        if (!ps.empty()) {
            Transform3D* pt = ps[0]->GetComponentOfType<Transform3D>();
            if (pt) {
                Vector3f pgs = pt->GetGlobalScale();
                gs = Vector3f(gs.x * pgs.x, gs.y * pgs.y, gs.z * pgs.z);

                Vector3f scaledLocal(
                    gp.x * pgs.x,
                    gp.y * pgs.y,
                    gp.z * pgs.z
                );
                Vector3f rp = pt->GetGlobalRotation().RotateVector(scaledLocal);
                gp = pt->GetGlobalPosition() + rp;

                gr = (pt->GetGlobalRotation() * gr).Normalized();
            }
        }
    }

    cachedMatrix = Matrix4x4f::Identity();

    float xx = gr.ratioVec.x * gr.ratioVec.x;
    float yy = gr.ratioVec.y * gr.ratioVec.y;
    float zz = gr.ratioVec.z * gr.ratioVec.z;
    float xy = gr.ratioVec.x * gr.ratioVec.y;
    float xz = gr.ratioVec.x * gr.ratioVec.z;
    float yz = gr.ratioVec.y * gr.ratioVec.z;
    float wx = gr.scalar * gr.ratioVec.x;
    float wy = gr.scalar * gr.ratioVec.y;
    float wz = gr.scalar * gr.ratioVec.z;

    cachedMatrix(0, 0) = (1 - 2 * (yy + zz)) * gs.x;
    cachedMatrix(0, 1) = (2 * (xy - wz)) * gs.x;
    cachedMatrix(0, 2) = (2 * (xz + wy)) * gs.x;
    cachedMatrix(0, 3) = gp.x;

    cachedMatrix(1, 0) = (2 * (xy + wz)) * gs.y;
    cachedMatrix(1, 1) = (1 - 2 * (xx + zz)) * gs.y;
    cachedMatrix(1, 2) = (2 * (yz - wx)) * gs.y;
    cachedMatrix(1, 3) = gp.y;

    cachedMatrix(2, 0) = (2 * (xz - wy)) * gs.z;
    cachedMatrix(2, 1) = (2 * (yz + wx)) * gs.z;
    cachedMatrix(2, 2) = (1 - 2 * (xx + yy)) * gs.z;
    cachedMatrix(2, 3) = gp.z;

    cachedMatrix(3, 0) = 0;
    cachedMatrix(3, 1) = 0;
    cachedMatrix(3, 2) = 0;
    cachedMatrix(3, 3) = 1;

    isDirty = false;
}

void Transform3D::Rotate(float pitch, float yaw, float roll) {
    Quaternionf qx = Quaternionf::CreateRotation(pitch, Vector3f(1, 0, 0));
    Quaternionf qy = Quaternionf::CreateRotation(yaw, Vector3f(0, 1, 0));
    Quaternionf qz = Quaternionf::CreateRotation(roll, Vector3f(0, 0, 1));
    fieldRotation = (qz * qy * qx * fieldRotation.GetValue()).Normalized();
    isDirty = true;
}

Vector3f Transform3D::Forward() const {
    return GetGlobalRotation().RotateVector(Vector3f(0, 0, 1));
}

Vector3f Transform3D::Backward() const {
    return -Forward();
}

Vector3f Transform3D::Right() const {
    return GetGlobalRotation().RotateVector(Vector3f(1, 0, 0));
}

Vector3f Transform3D::Left() const {
    return -Right();
}

Vector3f Transform3D::Up() const {
    return GetGlobalRotation().RotateVector(Vector3f(0, 1, 0));
}

Vector3f Transform3D::Down() const {
    return -Up();
}

void Transform3D::LookAt(const Vector3f& target) {
    Vector3f dir = target - GetGlobalPosition();
    if (dir.Magnitude() < 0.0001f) return;
    dir = dir.Normalized();

    Vector3f up = Vector3f(0, 1, 0);
    if (std::fabs(dir.Dot(up)) > 0.9999f) {
        up = Vector3f(1, 0, 0);
    }

    Vector3f z = dir;
    Vector3f x = up.Cross(z).Normalized();
    Vector3f y = z.Cross(x).Normalized();

    Matrix4x4f m;
    m(0,0) = x.x; m(0,1) = y.x; m(0,2) = z.x; m(0,3) = 0;
    m(1,0) = x.y; m(1,1) = y.y; m(1,2) = z.y; m(1,3) = 0;
    m(2,0) = x.z; m(2,1) = y.z; m(2,2) = z.z; m(2,3) = 0;
    m(3,0) = 0;   m(3,1) = 0;   m(3,2) = 0;   m(3,3) = 1;

    Quaternionf worldQ = Quaternionf::CreateFromMatrix(m);

    Quaternionf localQ = worldQ;
    HierarchySystem* sys = gameObject->GetHierarchySystem();
    if (sys) {
        std::vector<GameObject*> ps = sys->GetParents(gameObject);
        if (!ps.empty()) {
            Transform3D* pt = ps[0]->GetComponentOfType<Transform3D>();
            if (pt) {
                localQ = (pt->GetGlobalRotation().Inverse() * worldQ).Normalized();
            }
        }
    }

    fieldRotation = localQ;
    isDirty = true;
}

Matrix4x4f Transform3D::GetMatrix() const {
    if (isDirty) const_cast<Transform3D*>(this)->UpdateWorld();
    return cachedMatrix;
}

void Transform3D::Update() {
    if (isDirty) UpdateWorld();
}

Vector3f Transform3D::GetGlobalPosition() const {
    if (isDirty) const_cast<Transform3D*>(this)->UpdateWorld();
    return Vector3f(cachedMatrix(0, 3), cachedMatrix(1, 3), cachedMatrix(2, 3));
}

Vector3f Transform3D::GetGlobalScale() const {
    if (isDirty) const_cast<Transform3D*>(this)->UpdateWorld();
    Vector3f sx(cachedMatrix(0, 0), cachedMatrix(1, 0), cachedMatrix(2, 0));
    Vector3f sy(cachedMatrix(0, 1), cachedMatrix(1, 1), cachedMatrix(2, 1));
    Vector3f sz(cachedMatrix(0, 2), cachedMatrix(1, 2), cachedMatrix(2, 2));
    return Vector3f(sx.Magnitude(), sy.Magnitude(), sz.Magnitude());
}

Quaternionf Transform3D::GetGlobalRotation() const {
    if (isDirty) const_cast<Transform3D*>(this)->UpdateWorld();

    Vector3f sx(cachedMatrix(0, 0), cachedMatrix(1, 0), cachedMatrix(2, 0));
    Vector3f sy(cachedMatrix(0, 1), cachedMatrix(1, 1), cachedMatrix(2, 1));
    Vector3f sz(cachedMatrix(0, 2), cachedMatrix(1, 2), cachedMatrix(2, 2));

    float mx = sx.Magnitude();
    float my = sy.Magnitude();
    float mz = sz.Magnitude();
    if (mx > 0.0f) sx = sx / mx;
    if (my > 0.0f) sy = sy / my;
    if (mz > 0.0f) sz = sz / mz;

    Matrix4x4f m;
    m(0,0) = sx.x; m(0,1) = sy.x; m(0,2) = sz.x; m(0,3) = 0;
    m(1,0) = sx.y; m(1,1) = sy.y; m(1,2) = sz.y; m(1,3) = 0;
    m(2,0) = sx.z; m(2,1) = sy.z; m(2,2) = sz.z; m(2,3) = 0;
    m(3,0) = 0;    m(3,1) = 0;    m(3,2) = 0;    m(3,3) = 1;

    return Quaternionf::CreateFromMatrix(m);
}

const Vector3f& Transform3D::GetLocalPosition() const { return fieldPosition.GetValue(); }
const Vector3f& Transform3D::GetLocalScale() const    { return fieldScale.GetValue(); }
const Quaternionf& Transform3D::GetLocalRotation() const { return fieldRotation.GetValue(); }

void Transform3D::SetLocalPosition(const Vector3f& p) {
    fieldPosition = p;
    isDirty = true;
}

void Transform3D::SetLocalScale(const Vector3f& s) {
    fieldScale = s;
    isDirty = true;
}

void Transform3D::SetLocalRotation(const Quaternionf& r) {
    fieldRotation = r;
    isDirty = true;
}

void Transform3D::SetGlobalPositionAsLocal(const Vector3f& p, bool considerParents) {
    Quaternionf lr = fieldRotation.GetValue();
    Vector3f   ls = fieldScale.GetValue();
    Vector3f   lp = p;

    if (considerParents) {
        HierarchySystem* sys = gameObject ? gameObject->GetHierarchySystem() : nullptr;
        if (sys) {
            std::vector<GameObject*> ps = sys->GetParents(gameObject);
            if (!ps.empty()) {
                Transform3D* pt = ps[0]->GetComponentOfType<Transform3D>();
                if (pt) {
                    Vector3f diff   = p - pt->GetGlobalPosition();
                    Vector3f invRot = pt->GetGlobalRotation().Inverse().RotateVector(diff);
                    Vector3f pgs    = pt->GetGlobalScale();
                    lp = Vector3f(
                        pgs.x != 0.0f ? invRot.x / pgs.x : invRot.x,
                        pgs.y != 0.0f ? invRot.y / pgs.y : invRot.y,
                        pgs.z != 0.0f ? invRot.z / pgs.z : invRot.z
                    );

                    lr = (pt->GetGlobalRotation().Inverse() * lr).Normalized();
                    ls = Vector3f(
                        pgs.x != 0.0f ? ls.x / pgs.x : ls.x,
                        pgs.y != 0.0f ? ls.y / pgs.y : ls.y,
                        pgs.z != 0.0f ? ls.z / pgs.z : ls.z
                    );
                }
            }
        }
    }

    fieldPosition = lp;
    fieldRotation = lr;
    fieldScale    = ls;
    isDirty = true;
}

void Transform3D::SetGlobalScaleAsLocal(const Vector3f& s, bool considerParents) {
    Vector3f ls = s;

    if (considerParents) {
        HierarchySystem* sys = gameObject ? gameObject->GetHierarchySystem() : nullptr;
        if (sys) {
            std::vector<GameObject*> ps = sys->GetParents(gameObject);
            if (!ps.empty()) {
                Transform3D* pt = ps[0]->GetComponentOfType<Transform3D>();
                if (pt) {
                    Vector3f pgs = pt->GetGlobalScale();
                    ls = Vector3f(
                        pgs.x != 0.0f ? s.x / pgs.x : s.x,
                        pgs.y != 0.0f ? s.y / pgs.y : s.y,
                        pgs.z != 0.0f ? s.z / pgs.z : s.z
                    );
                }
            }
        }
    }

    fieldScale = ls;
    isDirty = true;
}

void Transform3D::SetGlobalRotationAsLocal(const Quaternionf& r, bool considerParents) {
    Quaternionf lr = r;

    if (considerParents) {
        HierarchySystem* sys = gameObject ? gameObject->GetHierarchySystem() : nullptr;
        if (sys) {
            std::vector<GameObject*> ps = sys->GetParents(gameObject);
            if (!ps.empty()) {
                Transform3D* pt = ps[0]->GetComponentOfType<Transform3D>();
                if (pt) {
                    lr = (pt->GetGlobalRotation().Inverse() * r).Normalized();
                }
            }
        }
    }

    fieldRotation = lr;
    isDirty = true;
}