#pragma once

#include "ModelFeatureType.h"

class ModelFeature{
public:
    ModelFeatureType type;
    virtual ~ModelFeature() = default;
};