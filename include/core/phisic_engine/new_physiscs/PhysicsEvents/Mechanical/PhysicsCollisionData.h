#pragma once

#include "PhysicsEvents.h"
#include "IntersectionSegments3D.h"
#include "GameObject.h"

class PhysicsCollisionData : public AbstractPhysicsData{
private:
    std::vector<Segment3D> segments;
    GameObject* otherObject;

public:
    const std::vector<Segment3D>& GetCollisionsSegments() const { 
        return segments;
    }

    const GameObject* GetCollisionsGameObject() const { 
        return otherObject;
    }

    void SetCollisionsSegments(std::vector<Segment3D>&& segments) {
        segments = std::move(segments);
    }

    void SetCollisionsGameObject(GameObject* otherObject) {
        this->otherObject = otherObject;
    }
};