#pragma once

#include <cmath>
#include "Math/Transform/Transform.h"
#include "Engine/Serialization/SerializeField.h"
#include "Math/Vector/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix/Matrix4x4.h"

class Transform3D : public Transform<Vector3f> {
private:
    FIELD(Vector3f, fieldPosition);
    FIELD(Vector3f, fieldScale);
    FIELD(Quaternionf, fieldRotation);

    bool dirty = true;
    Matrix4x4f cachedMatrix;

    void UpdateWorld();

public:
    Transform3D();

    void Rotate(float pitch, float yaw, float roll);

    Vector3f Forward() const;
    Vector3f Backward() const;
    Vector3f Right() const;
    Vector3f Left() const;
    Vector3f Up() const;
    Vector3f Down() const;

    void LookAt(const Vector3f& target);

    Matrix4x4f GetMatrix() const;

    void Update() override;

    Vector3f GetGlobalPosition() const override;
    Vector3f GetGlobalScale() const override;
    Quaternionf GetGlobalRotation() const override;

    const Vector3f& GetLocalPosition() const override;
    const Vector3f& GetLocalScale() const override;
    const Quaternionf& GetLocalRotation() const override;

    void SetLocalPosition(const Vector3f& p) override;
    void SetLocalScale(const Vector3f& s) override;
    void SetLocalRotation(const Quaternionf& r) override;

    void SetGlobalPositionAsLocal(const Vector3f& p) override;
    void SetGlobalScaleAsLocal(const Vector3f& s) override;
    void SetGlobalRotationAsLocal(const Quaternionf& r) override;

    bool IsDirty() const { return dirty; }

    SERIALIZED_FIELDS(&fieldPosition, &fieldScale, &fieldRotation)
};