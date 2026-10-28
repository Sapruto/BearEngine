#pragma once

#include "World/Components/Component.h"

class IRenderable : public Component {
protected:
    bool isActive{true};

    int layer{0};

public:
    virtual ~IRenderable() = default;

    int GetLayer() const{ return layer; }
    bool IsActive() const{ return isActive; }
};