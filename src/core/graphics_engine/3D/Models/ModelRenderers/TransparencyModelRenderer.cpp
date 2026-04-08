#include "TransparencyModelRenderer.h"

#include "ModelComponent.h"
#include "ModelRenderer.h"
#include "TransparentFeature.h"
#include "UVManager.h"
#include <glad/glad.h>
#include <iostream>

TransparencyModelRenderer::TransparencyModelRenderer() 
    : m_Shader("include/core/graphics_engine/3D/Models/Shaders/Transparent/Vertex.glsl", 
               "include/core/graphics_engine/3D/Models/Shaders/Transparent/Fragment.glsl"),
      m_ScreenQuadVAO(0),
      m_ScreenQuadVBO(0)
{
    type = ModelFeatureType::Transparency;
    InitScreenQuad();
    CacheUniformLocations();
}

void TransparencyModelRenderer::InitScreenQuad() {
    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_ScreenQuadVAO);
    glGenBuffers(1, &m_ScreenQuadVBO);
    
    glBindVertexArray(m_ScreenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_ScreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void TransparencyModelRenderer::RenderScreenQuad() {
    if (m_ScreenQuadVAO == 0) return;
    glBindVertexArray(m_ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void TransparencyModelRenderer::CacheUniformLocations() {
    m_Shader.Bind();
    m_Uniforms.inputTexture = glGetUniformLocation(m_Shader.GetID(), "inputTexture");
    m_Uniforms.baseColor = glGetUniformLocation(m_Shader.GetID(), "baseColor");
    m_Uniforms.opacity = glGetUniformLocation(m_Shader.GetID(), "opacity");
}

void TransparencyModelRenderer::RenderGroup(std::vector<ModelComponent*> models, ModelRenderer* baseRenderer){
    GLuint sceneTexture = baseRenderer->GetReadTexture();
    if (sceneTexture == 0) return;
    
    m_Shader.Bind();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glUniform1i(m_Uniforms.inputTexture, 0);
    
    glDisable(GL_DEPTH_TEST);
    
    glDisable(GL_BLEND);
    glUniform1f(m_Uniforms.opacity, 1.0f);
    RenderScreenQuad();
    
    if (!models.empty()) {
        for(auto* model : models){
            ObjectUVData uvData = baseRenderer->GetObjectUV(model);
            if (!uvData.isVisible || uvData.screenVertices.empty()) continue;
            
            float opacity = model->GetFeatureOfType<TransparentFeature>()->GetAlpha();
            
            GLuint vao, vbo, ebo;
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);
            
            glBindVertexArray(vao);
            
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, uvData.screenVertices.size() * sizeof(Vector2), 
                         uvData.screenVertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)0);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, uvData.indices.size() * sizeof(unsigned int), 
                         uvData.indices.data(), GL_STATIC_DRAW);
            
            glEnable(GL_BLEND);

            glBlendFunc(GL_ZERO, GL_ZERO);
            glUniform1f(m_Uniforms.opacity, 0.0f);
            glDrawElements(GL_TRIANGLES, uvData.indices.size(), GL_UNSIGNED_INT, 0);
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUniform1f(m_Uniforms.opacity, opacity);
            glDrawElements(GL_TRIANGLES, uvData.indices.size(), GL_UNSIGNED_INT, 0);
            
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }
        
        glDisable(GL_BLEND);
    }
    
    glEnable(GL_DEPTH_TEST);
}