#pragma once

#include "Math/Transform/Transform3D.h"
#include "Math/Vector/Vector3.h"
#include "Math/Geometry/AABB.h"
#include "../Core/IShape.h"

class IShape3D : public IShape {
protected:
    Transform3D* transform = nullptr;

public:
    void Start() override {
        transform = gameObject->GetComponentOfType<Transform3D>();
        //TODO when I added system with dependencies for components I use this in start
    }
    virtual ~IShape3D() = default;
    
    virtual Vector3f GetCenter() const = 0;
    virtual Vector3f GetGlobalCenter() const = 0;
    virtual const AABB_3D& GetAABB() const = 0;
    virtual const AABB_3D& GetGlobalAABB() const = 0;

    Transform3D* GetTransform() const { return transform; }
};