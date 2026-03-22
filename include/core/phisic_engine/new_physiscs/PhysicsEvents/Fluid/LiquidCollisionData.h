#pragma once

#include "PhysicsEvents.h"
#include "PhysicalBody.h"

#include "Vector3.h"

class LiquidCollisionData : public AbstractPhysicsData {
private:
    PhysicalBody* liquidBody;
    float volumeInLiquid;
    Vector3 direction;
    
public:
    LiquidCollisionData(PhysicalBody* liquidBody = nullptr, 
                        float volumeInLiquid = 1.0f, 
                        const Vector3& direction = Vector3::Up) 
        : liquidBody(liquidBody), 
          volumeInLiquid(volumeInLiquid), 
          direction(direction) {}

    ~LiquidCollisionData() = default;

    float GetVolumeInLiquid() { return volumeInLiquid; }
    PhysicalBody* GetLiquidBody() { return liquidBody; }

    Vector3 GetDirection() { return direction; }
};