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

TransparencyModelRenderer::TransparencyModelRenderer() 
    : m_Shader(ModelsShaderPaths::Base + "transparent/Vertex.glsl", 
               ModelsShaderPaths::Base + "transparent/Fragment.glsl"),
      m_ScreenQuadVAO(0),
      m_ScreenQuadVBO(0)
{
    type = ModelFeatureType::Transparency;
}

std::vector<ModelComponent*> TransparencyModelRenderer::BuildHierarchy(std::vector<ModelComponent*> models, Camera3D* camera){
    std::vector<ModelComponent*> sortingLayer = models;
    
    std::sort(sortingLayer.begin(), sortingLayer.end(), 
        [camera](ModelComponent* a, ModelComponent* b) {
            float distA = a->GetGameObject()->GetComponentOfType<Transform3D>()->GetPosition().distanceTo(camera->GetPosition());
            float distB = b->GetGameObject()->GetComponentOfType<Transform3D>()->GetPosition().distanceTo(camera->GetPosition());
            return distA > distB;
        });
    
    return sortingLayer;
}

void TransparencyModelRenderer::CacheUniformLocations() {
    m_Shader.Bind();
    m_Uniforms.inputTexture = glGetUniformLocation(m_Shader.GetID(), "inputTexture");
    m_Uniforms.baseColor = glGetUniformLocation(m_Shader.GetID(), "baseColor");
    m_Uniforms.opacity = glGetUniformLocation(m_Shader.GetID(), "opacity");
}

void TransparencyModelRenderer::Init() {
    if (isInitialized) return;

    CacheUniformLocations();
    isInitialized = true;
}

void TransparencyModelRenderer::Shutdown() {
    if (!isInitialized) return;

    isInitialized = false;
}

void TransparencyModelRenderer::RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) {
    if (!pipeline || !isInitialized || models.empty()) return;
    
    Camera3D* camera = pipeline->GetCamera();
    if (!camera) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ZERO);
    glDisable(GL_DEPTH_TEST);
    
    std::vector<ModelComponent*> sorted = BuildHierarchy(models, camera);
    ClearUVRegion(sorted, pipeline);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    for (auto* model : sorted) {
        if (!model) continue;
        
        const ObjectUVData& uvData = pipeline->GetObjectUV(model);
        if (!uvData.isVisible || uvData.screenVertices.empty()) continue;
        
        TransparentFeature* feature = model->GetFeatureOfType<TransparentFeature>();
        if (feature) {
            float opacity = feature->GetAlpha();
            m_Shader.SetFloat("opacity", opacity);
        }
        const float* color = model->GetColor();
        m_Shader.SetVec3("baseColor", color[0], color[1], color[2]);
        
        glBindVertexArray(model->GetVAO());
        glDrawElements(GL_TRIANGLES, uvData.indices.size(), GL_UNSIGNED_INT, 0);
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}