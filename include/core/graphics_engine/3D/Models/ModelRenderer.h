#pragma once

#include "Renderer.h"
#include "ModelComponent.h"
#include "Shader.h"

#include <vector>

class Transform3D;
class Camera3D;

class ModelRenderer : public Renderer{
private:
    Shader m_Shader;

    struct UniformLocations {
        int projection = -1;
        int view = -1;
        int model = -1;
        int color = -1;
    } m_Uniforms;

    void CacheUniformLocations();

public:
    ModelRenderer();
    ~ModelRenderer();

    void Update() override;

    void RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB, Camera3D* camera);
};