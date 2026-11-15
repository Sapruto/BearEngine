#pragma once

#include <cmath>
#include "Transform.h"
#include "World/Hierarchy/HierarchySystem.h"
#include "Math/Vector/Vector2.h"
#include "Math/Quaternion.h"
#include "Math/Matrix/Matrix4x4.h"

class Transform2D : public Transform<Vector2f> {
private:
    FIELD(Vector2f, fieldPosition);
    FIELD(Vector2f, fieldScale);
    FIELD(float, fieldRotation);

    Vector2f position;
    Vector2f scale;
    Quaternionf rotation;

    bool dirty;
    Matrix4x4f cachedMatrix;
    mutable Quaternionf cachedRotation;

    Transform2D combine(const Transform2D& parent) const;
    void updateWorld();
    float QuaternionfToAngle(const Quaternionf& q) const;
    Quaternionf angleToQuaternionf(float angle) const;

public:
    Transform2D();

    void rotate(float angleRad);
    Vector2f forward() const;
    Vector2f right() const;
    void lookAt(const Vector2f& target);

    Matrix4x4f GetMatrix() const;
    float GetAngle() const;

    void Update() override;

    Vector2f GetGlobalPosition() const override;
    Vector2f GetGlobalScale() const override;
    Quaternionf GetGlobalRotation() const override;

    const Vector2f& GetLocalPosition() const override;
    const Vector2f& GetLocalScale() const override;
    float GetLocalAngle() const;
    const Quaternionf& GetLocalRotation() const override;

    void SetLocalPosition(const Vector2f& newPos) override;
    void SetLocalScale(const Vector2f& newScale) override;
    void SetLocalAngle(float newAngle);
    void SetLocalRotation(const Quaternionf& newRot) override;

    void SetGlobalPositionAsLocal(const Vector2f& p, bool considerParents = false) override {
        (void)p;
    }

    void SetGlobalScaleAsLocal(const Vector2f& s, bool considerParents = false) override {
        (void)s;
    }

    void SetGlobalRotationAsLocal(const Quaternionf& r, bool considerParents = false) override {
        (void)r;
    }

    SERIALIZED_FIELDS(&fieldPosition, &fieldScale, &fieldRotation)
};