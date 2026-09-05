#include "RenderPipeline.h"

#include "ModelComponent.h"
#include "Camera3D.h"
#include "Window.h"
#include "Shader.h"
#include <glad/glad.h>
#include "ModelsShaderPaths.h"
#include <iostream>

RenderPipeline::RenderPipeline(Camera3D* cam, Window* win)
    : camera(cam), window(win), presentShader(
        ModelsShaderPaths::Base + "quad/Vertex.glsl",
        ModelsShaderPaths::Base + "quad/Fragment.glsl") {
    
    if (window) {
        width = window->GetWidth();
        height = window->GetHeight();
    }
    
    CreateFramebuffers(width, height);
    InitQuad();
}

RenderPipeline::~RenderPipeline() {
    DestroyFramebuffers();
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}

void RenderPipeline::CreateFramebuffers(int w, int h) {
    if (w <= 0 || h <= 0) return;
    
    width = w;
    height = h;

    for (int i = 0; i < 2; i++) {
        glGenTextures(1, &colorTextures[i]);
        glBindTexture(GL_TEXTURE_2D, colorTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenRenderbuffers(1, &depthBuffers[i]);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glGenFramebuffers(1, &framebuffers[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextures[i], 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers[i]);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer " << i << " is not complete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    currentRead = 0;
    currentWrite = 1;
}

void RenderPipeline::DestroyFramebuffers() {
    for (int i = 0; i < 2; i++) {
        if (framebuffers[i]) {
            glDeleteFramebuffers(1, &framebuffers[i]);
            framebuffers[i] = 0;
        }
        if (colorTextures[i]) {
            glDeleteTextures(1, &colorTextures[i]);
            colorTextures[i] = 0;
        }
        if (depthBuffers[i]) {
            glDeleteRenderbuffers(1, &depthBuffers[i]);
            depthBuffers[i] = 0;
        }
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
    if (!quadVAO) return;
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RenderPipeline::SwapBuffers() {
    std::swap(currentRead, currentWrite);
}

void RenderPipeline::BeginCurrentRender() {
    if (!framebuffers[currentWrite]) {
        std::cout << "Framebuffer is null!" << std::endl;
        return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[currentWrite]);
    glViewport(0, 0, width, height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderPipeline::EndCurrentRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    SwapBuffers();
}

void RenderPipeline::Present() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    
    presentShader.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTextures[currentRead]);
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
    
    DestroyFramebuffers();
    CreateFramebuffers(width, height);
}

ObjectUVData RenderPipeline::GetObjectUV(ModelComponent* model) {
    if (uvManager) {
        return uvManager->GetObjectUV(model);
    }
    return ObjectUVData();
}