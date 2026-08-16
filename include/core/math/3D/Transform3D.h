#pragma once

#include <cmath>
#include "Transform.h"
#include "HierarchySystem.h"
#include "SerializeField.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix/Matrix4x4.h"

class Transform3D : public Transform, public ISerializable {
private:
    FIELD(Vector3f, fieldPosition);
    FIELD(Vector3f, fieldScale);
    FIELD(Quaternionf, fieldRotation);

    Vector3f position;
    Vector3f scale;
    Quaternionf rotation;

    bool dirty = true;
    Matrix4x4f cachedMatrix;

    Transform3D combine(const Transform3D& parent) const;
    void updateWorld();

public:
    Transform3D();

    void rotate(float pitch, float yaw, float roll);

    Vector3f forward() const;
    Vector3f backward() const;
    Vector3f right() const;
    Vector3f left() const;
    Vector3f up() const;
    Vector3f down() const;

    void lookAt(const Vector3f& target);

    Matrix4x4f GetMatrix() const;

    void Update() override;

    const Vector3f& GetPosition() const;
    const Vector3f& GetScale() const;
    const Quaternionf& GetRotation() const;

    void SetPosition(const Vector3f& newWorldPos);
    void SetScale(const Vector3f& newWorldScale);
    void SetRotation(const Quaternionf& newWorldRot);

    const Vector3f& GetLocalPosition() const;
    const Vector3f& GetLocalScale() const;
    const Quaternionf& GetLocalRotation() const;

    void SetLocalPosition(const Vector3f& newPos);
    void SetLocalScale(const Vector3f& newScale);
    void SetLocalRotation(const Quaternionf& newRot);

    SERIALIZED_FIELDS(&fieldPosition, &fieldScale, &fieldRotation)
};