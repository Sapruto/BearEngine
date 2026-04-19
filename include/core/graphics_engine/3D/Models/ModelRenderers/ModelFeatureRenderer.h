#pragma once

#include "ModelFeatureType.h"

#include <vector> 
#include <glad/glad.h>

class ModelComponent;
class ModelRenderer;

class ModelFeatureRenderer{
public:
    ModelFeatureType type;

    virtual void RenderGroup(std::vector<ModelComponent*> models, 
                             ModelRenderer* baseRenderer) = 0; 
};