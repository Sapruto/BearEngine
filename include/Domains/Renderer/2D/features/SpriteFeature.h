#pragma once

#include "SpriteFeatureType.h"

class SpriteFeature {
public:
    SpriteFeatureType type;
    virtual ~SpriteFeature() = default;
};