#include "ModelRenderer.h"
#include "GraphicsManager.h"
#include "Transform3D.h"
#include "Camera3D.h"
#include "DirectionalLight3D.h"
#include "SimpleModelRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

ModelRenderer::ModelRenderer()
    : m_Shader("include/core/graphics_engine/3D/Models/Shaders/QuadShader/Vertex.glsl", "include/core/graphics_engine/3D/Models/Shaders/QuadShader/Fragment.glsl")
{
    InitQuad();
    InitBaseRenderer();
}

ModelRenderer::~ModelRenderer(){

}

void ModelRenderer::InitQuad(){
    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void ModelRenderer::CreateFramebuffers(int width, int height){
    textureWidth = width;
    textureHeight = height;
    
    for (int i = 0; i < 2; i++){
        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glGenRenderbuffers(1, &depthBuffers[i]);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        
        glGenFramebuffers(1, &framebuffers[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers[i]);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    currentRead = 0;
    currentWrite = 1;
}

void ModelRenderer::RenderResultTexture(GLuint texture){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_Shader.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    m_Shader.SetInt("screenTexture", 0);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    m_Shader.Unbind();
}

void ModelRenderer::InitBaseRenderer(){
    //RegisterModelFeatureRenderer(new SimpleModelRenderer());
    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        groups[ModelFeatureType::Simple].push_back(model);
    }
}

void ModelRenderer::BuildGroups(){
    if(!isDirty) return;

    for(auto& [type, group] : groups){
        group.clear();
    }

    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        std::vector<ModelFeature*> features = model->GetFeatures();
        for(auto* feature : features){
            groups[feature->type].push_back(model);
        }
    }
    InitBaseRenderer();
    isDirty = false;
}

void ModelRenderer::Update(){
    BuildGroups();
    
    static int uvUpdateCounter = 0;
    uvUpdateCounter++;
    if (uvUpdateCounter % 30 == 0) {
        UpdateUVs();
        uvUpdateCounter = 0;
    }

    int width, height;
    glfwGetWindowSize(manager->GetWindow()->GetWindow(), &width, &height);
    
    if(framebuffers[0] == 0 || width != textureWidth || height != textureHeight){
        if (framebuffers[0] != 0) {
            for (int i = 0; i < 2; i++) {
                glDeleteFramebuffers(1, &framebuffers[i]);
                glDeleteTextures(1, &textures[i]);
                glDeleteRenderbuffers(1, &depthBuffers[i]);
            }
        }
        CreateFramebuffers(width, height);
    }
    
    currentRead = 0;
    currentWrite = 1;
    
    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
        if(!modelFeatureRenderer || !groups.count(type)) continue;
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[currentWrite]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        modelFeatureRenderer->RenderGroup(groups[type], this);
        
        std::swap(currentRead, currentWrite);
    }
        
    RenderResultTexture(textures[currentRead]);
}

void ModelRenderer::RemoveLight(Light3D* light) {
    auto it = std::find_if(lights.begin(), lights.end(),
        [light](const std::unique_ptr<Light3D>& li) {
            return li.get() == light;
        });
    
    if (it != lights.end()) {
        lights.erase(it);
    }
}