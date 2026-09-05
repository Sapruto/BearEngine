#pragma once

#include "ModelFeatureRenderer.h"
#include "Shader.h"
#include "ModelFeatureType.h"

#include <vector>
#include <memory>
#include <glad/glad.h>

class ModelComponent;
class ModelRenderer;
class Camera3D;

class TransparencyModelRenderer : public ModelFeatureRenderer {
private:
    Shader shader;
    
    struct UniformsName {
        static const std::string inputTexture;
        static const std::string baseColor;
        static const std::string opacity;
    };

    std::vector<ModelComponent*> BuildHierarchy(std::vector<ModelComponent*> models, Camera3D* camera);
    void RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB, float opacity);

public:
    TransparencyModelRenderer();
    ~TransparencyModelRenderer() override {}
    
    void Init() override;
    void Shutdown() override;

    void RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) override;
};