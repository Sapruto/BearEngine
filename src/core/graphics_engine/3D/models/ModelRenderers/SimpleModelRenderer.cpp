#include "SimpleModelRenderer.h"

#include "Model.h"
#include "ModelComponent.h"
#include "ModelRenderer.h"
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

const std::string SimpleModelRenderer::UniformsName::projection = "projection";
const std::string SimpleModelRenderer::UniformsName::view = "view";
const std::string SimpleModelRenderer::UniformsName::viewPos = "viewPos";
const std::string SimpleModelRenderer::UniformsName::color = "color";
const std::string SimpleModelRenderer::UniformsName::lightCount = "lightCount";
const std::string SimpleModelRenderer::UniformsName::dirLightDirection = "dirLightDirection";
const std::string SimpleModelRenderer::UniformsName::dirLightColor = "dirLightColor";
const std::string SimpleModelRenderer::UniformsName::useDirLight = "useDirLight";
const std::string SimpleModelRenderer::UniformsName::lightPosPrefix = "lightPos";
const std::string SimpleModelRenderer::UniformsName::lightColorPrefix = "lightColor";
const std::string SimpleModelRenderer::UniformsName::lightSpaceMatrixPrefix = "lightSpaceMatrix";
const std::string SimpleModelRenderer::UniformsName::model = "model";

SimpleModelRenderer::SimpleModelRenderer() 
    : shader(ModelsShaderPaths::Base + "simple/Vertex.glsl",
             ModelsShaderPaths::Base + "simple/Fragment.glsl")
{
    type = ModelFeatureType::Simple;
}

void SimpleModelRenderer::RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB) {
    shader.SetMat4(UniformsName::model, transform->GetMatrix());
    shader.SetVec3(UniformsName::color, colorRGB[0], colorRGB[1], colorRGB[2]);
    
    glBindVertexArray(modelComp->GetVAO());
    glDrawElements(GL_TRIANGLES, modelComp->GetModel()->GetIndices().size(), 
                   GL_UNSIGNED_INT, 0);
}

void SimpleModelRenderer::Init() {
    if (isInitialized) return;

    std::cout << "SimpleModelRenderer::Init - compiling shaders..." << std::endl;
    
    shader.Bind();
    std::cout << "Shader ID: " << shader.GetID() << std::endl;
    
    if (shader.GetID() == 0) {
        std::cout << "SHADER COMPILATION FAILED!" << std::endl;
        return;
    }
    
    isInitialized = true;
    std::cout << "SimpleModelRenderer initialized successfully!" << std::endl;
}

void SimpleModelRenderer::Shutdown() {
    if (!isInitialized) return;

    isInitialized = false;
}

void SimpleModelRenderer::RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) {
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