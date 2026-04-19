#pragma once

#include "ModelFeatureRenderer.h"
#include "Shader.h"

#include <vector>
#include <memory>
#include <algorithm> 

class ModelComponent;
class ModelRenderer;
class Transform3D;

class SimpleModelRenderer : public ModelFeatureRenderer{
private:
    Shader m_Shader;

    struct UniformLocations {
        int projection = -1;
        int view = -1;
        int model = -1;
        int color = -1;
    } m_Uniforms;

public:
    SimpleModelRenderer();

    void CacheUniformLocations();

    void RenderGroup(std::vector<ModelComponent*> models, ModelRenderer* baseRenderer) override;
    void RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB);
};