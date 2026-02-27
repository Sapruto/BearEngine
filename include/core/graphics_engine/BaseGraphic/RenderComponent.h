#pragma once

#include "include/core/system_engine/component_system/Component.h"
#include "src/dependencies/glm/GLM/glm.hpp"

class RenderComponent : public Component{
public:
    virtual ~RenderComponent() = default;
    
    virtual void Render(const glm::vec2& position, float rotation) = 0;
    virtual bool IsVisible() const = 0;
    virtual int GetLayer() const = 0;
};