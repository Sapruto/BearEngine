#pragma once

#include <cmath>
#include "Transform.h"
#include "HierarchySystem.h"
#include "SerializeField.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix/Matrix4x4.h"

class Transform2D : public Transform, public ISerializable {
private:
    FIELD(Vector2f, fieldPosition);
    FIELD(Vector2f, fieldScale);
    FIELD(float, fieldRotation);

    Vector2f position;
    Vector2f scale;
    Quaternionf rotation;

    bool dirty;
    Matrix4x4f cachedMatrix;

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

    const Vector2f& GetPosition() const;
    const Vector2f& GetScale() const;
    const Quaternionf& GetRotation() const;

    void SetPosition(const Vector2f& newWorldPos);
    void SetScale(const Vector2f& newWorldScale);
    void SetRotation(const Quaternionf& newWorldRot);
    void SetAngle(float newWorldAngle);

    const Vector2f& GetLocalPosition() const;
    const Vector2f& GetLocalScale() const;
    float GetLocalAngle() const;
    const Quaternionf& GetLocalRotation() const;

    void SetLocalPosition(const Vector2f& newPos);
    void SetLocalScale(const Vector2f& newScale);
    void SetLocalAngle(float newAngle);
    void SetLocalRotation(const Quaternionf& newRot);

    SERIALIZED_FIELDS(&fieldPosition, &fieldScale, &fieldRotation)
};