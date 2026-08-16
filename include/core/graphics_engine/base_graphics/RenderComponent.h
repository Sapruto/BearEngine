#pragma once

#include "Component.h"
#include <glm/glm.hpp>

class RenderComponent : public Component{
protected:
    bool isActive{true};

    int layer{0};

public:
    virtual ~RenderComponent() = default;

    int GetLayer() const{ return layer; }
    bool IsActive() const{ return isActive; }
};