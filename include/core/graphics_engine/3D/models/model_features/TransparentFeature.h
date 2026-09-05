#pragma once

#include "ModelFeature.h"
#include "ModelFeatureType.h"

class TransparentFeature : public ModelFeature {
private:
    float alpha;
public:
    TransparentFeature() { type = ModelFeatureType::Transparency; }
    
    void SetAlpha(float a) { alpha = a; }
    float GetAlpha() const { return alpha; }
};