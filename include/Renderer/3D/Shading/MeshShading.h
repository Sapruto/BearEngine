#pragma once

#include "MeshShadingType.h"

class MeshShading {
public:
    MeshShadingType type;
    virtual ~MeshShading() = default;
};