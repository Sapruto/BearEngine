#pragma once

#include "SpriteFeature.h"
#include "SpriteFeatureType.h"

class TransparentFeature2D : public SpriteFeature {
private:
    float alpha = 0.5f;
    
public:
    TransparentFeature2D() { type = SpriteFeatureType::Transparent; }
    
    void SetAlpha(float a) { alpha = a; }
    float GetAlpha() const { return alpha; }
};