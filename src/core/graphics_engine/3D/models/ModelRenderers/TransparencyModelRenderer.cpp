#include "TransparencyModelRenderer.h"

#include "ModelComponent.h"
#include "ModelRenderer.h"
#include "TransparentFeature.h"
#include "UVManager.h"
#include <glad/glad.h>
#include <iostream>

#include "Camera3D.h"
#include "Transform3D.h"

#include "ModelsShaderPaths.h"

const std::string TransparencyModelRenderer::UniformsName::inputTexture = "inputTexture";
const std::string TransparencyModelRenderer::UniformsName::baseColor = "baseColor";
const std::string TransparencyModelRenderer::UniformsName::opacity = "opacity";

TransparencyModelRenderer::TransparencyModelRenderer() 
    : shader(ModelsShaderPaths::Base + "transparent/Vertex.glsl", 
             ModelsShaderPaths::Base + "transparent/Fragment.glsl")
{
    type = ModelFeatureType::Transparency;
}

std::vector<ModelComponent*> TransparencyModelRenderer::BuildHierarchy(std::vector<ModelComponent*> models, Camera3D* camera) {
    std::vector<ModelComponent*> sortingLayer = models;
    
    std::sort(sortingLayer.begin(), sortingLayer.end(), 
        [camera](ModelComponent* a, ModelComponent* b) {
            float distA = a->GetGameObject()->GetComponentOfType<Transform3D>()->GetGlobalPosition().distanceTo(camera->GetGlobalPosition());
            float distB = b->GetGameObject()->GetComponentOfType<Transform3D>()->GetGlobalPosition().distanceTo(camera->GetGlobalPosition());
            return distA > distB;
        });
    
    return sortingLayer;
}

void TransparencyModelRenderer::RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB, float opacity) {
    shader.SetVec3(UniformsName::baseColor, colorRGB[0], colorRGB[1], colorRGB[2]);
    shader.SetFloat(UniformsName::opacity, opacity);
    
    glBindVertexArray(modelComp->GetVAO());
    glDrawElements(GL_TRIANGLES, modelComp->GetModel()->GetIndices().size(), 
                   GL_UNSIGNED_INT, 0);
}

void TransparencyModelRenderer::Init() {
    if (isInitialized) return;

    std::cout << "TransparencyModelRenderer::Init - compiling shaders..." << std::endl;
    
    shader.Bind();
    std::cout << "Shader ID: " << shader.GetID() << std::endl;
    
    if (shader.GetID() == 0) {
        std::cout << "SHADER COMPILATION FAILED!" << std::endl;
        return;
    }
    
    isInitialized = true;
    std::cout << "TransparencyModelRenderer initialized successfully!" << std::endl;
}

void TransparencyModelRenderer::Shutdown() {
    if (!isInitialized) return;

    isInitialized = false;
}

void TransparencyModelRenderer::RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) {
    if (!pipeline || !isInitialized || models.empty()) return;
    
    Camera3D* camera = pipeline->GetCamera();
    if (!camera) return;
    
    std::vector<ModelComponent*> sorted = BuildHierarchy(models, camera);
    
    shader.Bind();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ZERO);
    glDisable(GL_DEPTH_TEST);
    
    ClearUVRegion(sorted, pipeline);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    for (auto* model : sorted) {
        if (!model) continue;
        
        const ObjectUVData& uvData = pipeline->GetObjectUV(model);
        if (!uvData.isVisible || uvData.screenVertices.empty()) continue;
        
        GameObject* object = model->GetGameObject();
        if (!object) continue;
        
        Transform3D* transform = object->GetComponentOfType<Transform3D>();
        if (!transform) continue;
        
        TransparentFeature* feature = model->GetFeatureOfType<TransparentFeature>();
        float opacity = feature ? feature->GetAlpha() : 1.0f;
        
        const float* color = model->GetColor();
        
        RenderModel(model, transform, color, opacity);
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}