#pragma once

#include "IRenderer.h"
#include "Shader.h"
#include "MeshShadingType.h"

#include <vector>
#include <memory>
#include <glad/glad.h>

class Mesh;
class RendererScene;
class Camera3D;
class Transform3D;

class TransparencyRenderer : public IRenderer {
private:
    Shader shader;
    
    struct UniformsName {
        static const std::string inputTexture;
        static const std::string baseColor;
        static const std::string opacity;
    };

    std::vector<Mesh*> BuildHierarchy(std::vector<Mesh*> models, Camera3D* camera);
    void RenderModel(Mesh* modelComp, Transform3D* transform, const float* colorRGB, float opacity);

public:
    TransparencyRenderer();
    ~TransparencyRenderer() override {}
    
    void Init() override;
    void Shutdown() override;

    void RenderGroup(std::vector<Mesh*>& models, RenderPipeline* pipeline) override;
};