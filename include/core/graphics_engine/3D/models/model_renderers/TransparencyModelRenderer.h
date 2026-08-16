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
    Shader m_Shader;
    GLuint m_ScreenQuadVAO;
    GLuint m_ScreenQuadVBO;

    struct UniformLocations {
        GLuint inputTexture;
        GLuint baseColor;
        GLuint opacity;
    } m_Uniforms;

    std::vector<ModelComponent*> BuildHierarchy(std::vector<ModelComponent*> models, Camera3D* camera);

    void CacheUniformLocations();

public:
    TransparencyModelRenderer();
    ~TransparencyModelRenderer() override {}
    
    void Init() override;
    void Shutdown() override;

    void RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) override;
};