#pragma once

#include "MeshShading.h"
#include "MeshShadingType.h"

class TransparentShader : public MeshShading {
private:
    float alpha;
public:
    TransparentShader() { type = MeshShadingType::Transparency; }
    
    void SetAlpha(float a) { alpha = a; }
    float GetAlpha() const { return alpha; }
};