#pragma once

#include <vector>
#include "RenderPipeline2D.h"
#include "SpriteFeatureType.h"

class SpriteComponent;

class BaseRenderer2D {
protected:
    SpriteFeatureType type;
    bool isInitialized = false;
    
public:
    virtual ~BaseRenderer2D() = default;
    
    SpriteFeatureType GetType() const { return type; }
    bool IsInitialized() const { return isInitialized; }
    
    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    virtual void RenderGroup(const std::vector<SpriteComponent*>& sprites, RenderPipeline2D* pipeline) = 0;
};