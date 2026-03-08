#pragma once

#include "ModelRenderer.h"
#include "GraphicsManager.h"
#include "Transform3D.h"
#include "Camera3D.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

ModelRenderer::ModelRenderer() 
    : m_Shader("include/core/graphics_engine/Shaders/ModelsShaders/Vertex.glsl", "include/core/graphics_engine/Shaders/ModelsShaders/Fragment.glsl")
{
    CacheUniformLocations();
}

ModelRenderer::~ModelRenderer(){

}

void ModelRenderer::CacheUniformLocations() {
    m_Shader.Bind();
    m_Uniforms.projection = glGetUniformLocation(m_Shader.GetID(), "projection");
    m_Uniforms.view = glGetUniformLocation(m_Shader.GetID(), "view");
    m_Uniforms.model = glGetUniformLocation(m_Shader.GetID(), "model");
    m_Uniforms.color = glGetUniformLocation(m_Shader.GetID(), "color");
}

void ModelRenderer::Update(){
    if (!manager) return;

    glEnable(GL_DEPTH_TEST);
    
    Camera3D* camera = dynamic_cast<Camera3D*>(manager->GetCamera());
    if (!camera) return;
    
    m_Shader.Bind();
    
    if (m_Uniforms.projection != -1) {
        int width, height;
        glfwGetWindowSize(manager->GetWindow()->GetWindow(), &width, &height);
        float aspect = (float)width / (float)height;

        glUniformMatrix4fv(m_Uniforms.projection, 1, GL_FALSE, 
                        glm::value_ptr(camera->GetProjectionMatrix(aspect)));
    }
    
    if (m_Uniforms.view != -1) {
        glUniformMatrix4fv(m_Uniforms.view, 1, GL_FALSE, 
                          glm::value_ptr(camera->GetViewMatrix()));
    }
    
    glUniform3f(glGetUniformLocation(m_Shader.GetID(), "lightPos"), 5.0f, 10.0f, 5.0f);
    glUniform3f(glGetUniformLocation(m_Shader.GetID(), "viewPos"), 
                camera->position.x, camera->position.y, camera->position.z);
    
    glActiveTexture(GL_TEXTURE0);

    std::vector<RenderComponent*> toRemove;

    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component); 
        if(!model){
            toRemove.push_back(model);
            continue;
        }

        GameObject* object = model->gameObject;
        if(!object){
            toRemove.push_back(model);
            continue;
        }

        Transform3D* transform = object->GetComponentOfType<Transform3D>();
        if(!transform){
            toRemove.push_back(model);
            continue;
        }

        Model* modelRefernce = model->GetModel();
        if(!modelRefernce){
            toRemove.push_back(model);
            continue;
        }

        const float* colorRGB = model->GetColor();

        RenderModel(model, transform, colorRGB, camera);
    }

    for (auto* component : toRemove) {
        auto it = std::find(rendererComponents.begin(), rendererComponents.end(), component);
        if (it != rendererComponents.end()) {
            rendererComponents.erase(it);
        }
    }
}

void ModelRenderer::RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB, Camera3D* camera) {
    if (m_Uniforms.model != -1) {
        glm::mat4 modelMatrix = transform->GetMatrix();
        glUniformMatrix4fv(m_Uniforms.model, 1, GL_FALSE, 
                          glm::value_ptr(modelMatrix));
    }
    
    if (m_Uniforms.color != -1) {
        glUniform3f(m_Uniforms.color, colorRGB[0], colorRGB[1], colorRGB[2]);
    }
    
    glBindVertexArray(modelComp->GetVAO());
    glDrawElements(GL_TRIANGLES, modelComp->GetModel()->GetIndices().size(), 
                   GL_UNSIGNED_INT, 0);
}