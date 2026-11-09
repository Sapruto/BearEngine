#pragma once

#include "BaseCollider.h"

class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;

    virtual bool HasCollision(BaseCollider* a, BaseCollider* b) = 0;
};