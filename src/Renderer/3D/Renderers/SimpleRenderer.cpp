#include "SimpleRenderer.h"

#include "Model.h"
#include "ModelComponent.h"
#include "RendererScene.h"
#include "ModelFeatureRenderer.h"

#include "Shader.h"
#include "ModelFeatureType.h"

#include "Light3D.h"
#include "DirectionalLight3D.h"

#include "Transform3D.h"
#include "Camera3D.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"

#include "ModelFeatureType.h"

#include <vector>
#include <memory>
#include <algorithm> 

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ModelsShaderPaths.h"

const std::string SimpleRenderer::UniformsName::projection = "projection";
const std::string SimpleRenderer::UniformsName::view = "view";
const std::string SimpleRenderer::UniformsName::viewPos = "viewPos";
const std::string SimpleRenderer::UniformsName::color = "color";
const std::string SimpleRenderer::UniformsName::lightCount = "lightCount";
const std::string SimpleRenderer::UniformsName::dirLightDirection = "dirLightDirection";
const std::string SimpleRenderer::UniformsName::dirLightColor = "dirLightColor";
const std::string SimpleRenderer::UniformsName::useDirLight = "useDirLight";
const std::string SimpleRenderer::UniformsName::lightPosPrefix = "lightPos";
const std::string SimpleRenderer::UniformsName::lightColorPrefix = "lightColor";
const std::string SimpleRenderer::UniformsName::lightSpaceMatrixPrefix = "lightSpaceMatrix";
const std::string SimpleRenderer::UniformsName::model = "model";

SimpleRenderer::SimpleRenderer()
    : shader(ModelsShaderPaths::Base + "simple/Vertex.glsl",
             ModelsShaderPaths::Base + "simple/Fragment.glsl")
{
    type = ModelFeatureType::Simple;
}

void SimpleRenderer::RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB) {
    shader.SetMat4(UniformsName::model, transform->GetMatrix());
    shader.SetVec3(UniformsName::color, colorRGB[0], colorRGB[1], colorRGB[2]);
    
    glBindVertexArray(modelComp->GetVAO());
    glDrawElements(GL_TRIANGLES, modelComp->GetModel()->GetIndices().size(), 
                   GL_UNSIGNED_INT, 0);
}

void SimpleRenderer::Init() {
    if (isInitialized) return;

    std::cout << "SimpleRenderer::Init - compiling shaders..." << std::endl;
    
    shader.Bind();
    std::cout << "Shader ID: " << shader.GetID() << std::endl;
    
    if (shader.GetID() == 0) {
        std::cout << "SHADER COMPILATION FAILED!" << std::endl;
        return;
    }
    
    isInitialized = true;
    std::cout << "SimpleRenderer initialized successfully!" << std::endl;
}

void SimpleRenderer::Shutdown() {
    if (!isInitialized) return;

    isInitialized = false;
}

void SimpleRenderer::RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) {
    if (!pipeline || !isInitialized) return;
    
    Camera3D* camera = dynamic_cast<Camera3D*>(pipeline->GetCamera());
    if (!camera) return;

    const auto& lights = pipeline->GetLights();
    
    shader.Bind();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    float aspect = (float)pipeline->GetWidth() / pipeline->GetHeight();

    Matrix4x4f proj = camera->GetProjectionMatrix(aspect);
    Matrix4x4f view = camera->GetViewMatrix();

    shader.SetMat4(UniformsName::projection, proj);
    shader.SetMat4(UniformsName::view, view);
    shader.SetInt(UniformsName::lightCount, (int)lights.size());

    bool hasDirLight = false;
    for (const auto* light : lights) {
        DirectionalLight3D* dirLight = dynamic_cast<DirectionalLight3D*>(
            const_cast<Light3D*>(light)
        );
        if (dirLight) {
            shader.SetVec3(UniformsName::dirLightDirection, dirLight->GetDirection());
            shader.SetVec3(UniformsName::dirLightColor, light->GetColor());
            
            hasDirLight = true;
            break;
        }
    }
    shader.SetInt(UniformsName::useDirLight, hasDirLight ? 1 : 0);

    for (size_t i = 0; i < lights.size(); ++i) {
        std::string posName = UniformsName::lightPosPrefix + "[" + std::to_string(i) + "]";
        shader.SetVec3(posName, lights[i]->GetGlobalPosition());
        
        std::string colName = UniformsName::lightColorPrefix + "[" + std::to_string(i) + "]";
        shader.SetVec3(colName, lights[i]->GetColor());
        
        std::string matrixName = UniformsName::lightSpaceMatrixPrefix + "[" + std::to_string(i) + "]";
        shader.SetMat4(matrixName, lights[i]->GetLightSpaceMatrix(aspect));
    }

    shader.SetVec3(UniformsName::viewPos, camera->GetGlobalPosition());
    
    glActiveTexture(GL_TEXTURE0);

    for (auto* model : models) {
        GameObject* object = model->GetGameObject();
        if (!object) {
            continue;
        }

        Transform3D* transform = object->GetComponentOfType<Transform3D>();
        if (!transform) {
            continue;
        }

        Model* modelReference = model->GetModel();
        if (!modelReference) {
            continue;
        }

        const float* colorRGB = model->GetColor();

        RenderModel(model, transform, colorRGB);
    }
}