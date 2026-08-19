#include "RenderPipeline.h"

#include "ModelComponent.h"
#include "Camera3D.h"
#include "Window.h"
#include "Shader.h"
#include <glad/glad.h>

#include "ModelsShaderPaths.h"

RenderPipeline::RenderPipeline(Camera3D* cam, Window* win)
    : camera(cam), window(win), presentShader(
        ModelsShaderPaths::Base + "quad/Vertex.glsl",
        ModelsShaderPaths::Base + "quad/Fragment.glsl") {
    
    if (window) {
        width = window->GetWidth();
        height = window->GetHeight();
    }
    
    CreateFramebuffer(width, height);
    InitQuad();
}

RenderPipeline::~RenderPipeline() {
    DestroyFramebuffer();
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}

void RenderPipeline::CreateFramebuffer(int w, int h) {
    if (w <= 0 || h <= 0) return;
    
    width = w;
    height = h;
    
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {

    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPipeline::DestroyFramebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    if (colorTexture) {
        glDeleteTextures(1, &colorTexture);
        colorTexture = 0;
    }
    if (depthBuffer) {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
}

void RenderPipeline::InitQuad() {
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

void RenderPipeline::RenderQuad() {
    if (!quadVAO) {
        std::cout << "quadVAO is nullptr in RenderPipeline" << std::endl; 
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RenderPipeline::BeginFrame() {
    if (!framebuffer) {
        std::cout << "framebuffer is nullptr in RenderPipeline" << std::endl; 
        return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderPipeline::EndFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPipeline::Present() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    
    presentShader.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    presentShader.SetInt("screenTexture", 0);
    
    RenderQuad();
    
    presentShader.Unbind();
}

void RenderPipeline::RemoveLight(Light3D* light) {
    auto it = std::find(lights.begin(), lights.end(), light);
    if (it != lights.end()) {
        lights.erase(it);
    }
}

void RenderPipeline::Resize(int w, int h) {
    if (w <= 0 || h <= 0) return;
    
    width = w;
    height = h;
    
    DestroyFramebuffer();
    CreateFramebuffer(width, height);
}

ObjectUVData RenderPipeline::GetObjectUV(ModelComponent* model) {
    if (uvManager) {
        return uvManager->GetObjectUV(model);
    }
    return ObjectUVData();
}