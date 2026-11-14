#pragma once

#include "../Quaternion.h"
#include "../Vector/BaseVector.h"
#include "World/Components/Component.h"

template<typename VectorT>
    requires (std::is_base_of_v<Vector, VectorT>)
class Transform : public Component {
public:
    virtual VectorT GetGlobalPosition() const = 0;
    virtual VectorT GetGlobalScale() const = 0;
    virtual Quaternionf GetGlobalRotation() const = 0;

    virtual const VectorT& GetLocalPosition() const = 0;
    virtual const VectorT& GetLocalScale() const = 0;
    virtual const Quaternionf& GetLocalRotation() const = 0;

    virtual void SetLocalPosition(const VectorT& p) = 0;
    virtual void SetLocalScale(const VectorT& s) = 0;
    virtual void SetLocalRotation(const Quaternionf& r) = 0;

    virtual void SetGlobalPositionAsLocal(const VectorT& p, bool considerParents = false) = 0;
    virtual void SetGlobalScaleAsLocal(const VectorT& s, bool considerParents = false) = 0;
    virtual void SetGlobalRotationAsLocal(const Quaternionf& r, bool considerParents = false) = 0;
};