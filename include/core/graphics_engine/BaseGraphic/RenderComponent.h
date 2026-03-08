#pragma once

#include "Component.h"
#include <glm.hpp>

class RenderComponent : public Component{
public:
    virtual ~RenderComponent() = default;
    
    virtual void Render(const glm::vec2& position, float rotation) = 0;
    virtual bool IsVisible() const = 0;
    virtual int GetLayer() const = 0;
};