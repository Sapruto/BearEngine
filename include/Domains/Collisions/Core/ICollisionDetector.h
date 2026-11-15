#pragma once

#include "IShape.h"
#include "ShapeRegistry.h"
#include <vector>

struct CollisionInfo;

class ICollisionDetector {
private:
    ShapeRegistry& registry;

protected:
    ShapeRegistry& GetRegistry() { return registry; }

public:
    explicit ICollisionDetector(ShapeRegistry& registry_) : registry(registry_) {}
    virtual ~ICollisionDetector() = default;

    virtual void ClearInfo() = 0;
    virtual bool HasCollision(ShapeId a, ShapeId b) = 0;

    virtual const std::vector<CollisionInfo>& GetCollisions() const = 0;
};