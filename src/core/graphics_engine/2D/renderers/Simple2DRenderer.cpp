#include "Simple2DRenderer.h"

#include "SpriteComponent.h"
#include "RenderPipeline2D.h"
#include "Camera2D.h"
#include "Transform2D.h"
#include <glad/glad.h>

Simple2DRenderer::Simple2DRenderer()
    : shader("include/core/graphics_engine/2D/Shaders/SpriteVertex.glsl",
             "include/core/graphics_engine/2D/Shaders/SpriteFragment.glsl") {
    type = SpriteFeatureType::Simple;
}

void Simple2DRenderer::Init() {
    if (isInitialized) return;
    shader.Bind();
    CacheUniformLocations();
    isInitialized = true;
}

void Simple2DRenderer::Shutdown() {
    isInitialized = false;
}

void Simple2DRenderer::CacheUniformLocations() {
    uniforms.projection = glGetUniformLocation(shader.GetID(), "projection");
    uniforms.view = glGetUniformLocation(shader.GetID(), "view");
    uniforms.model = glGetUniformLocation(shader.GetID(), "model");
    uniforms.texture = glGetUniformLocation(shader.GetID(), "texture1");
    uniforms.color = glGetUniformLocation(shader.GetID(), "spriteColor");
}

void Simple2DRenderer::RenderGroup(const std::vector<SpriteComponent*>& sprites, RenderPipeline2D* pipeline) {
    if (!pipeline || !isInitialized || sprites.empty()) return;
    
    Camera2D* camera = pipeline->GetCamera();
    if (!camera) return;
    
    shader.Bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    Matrix4x4f proj = camera->GetProjectionMatrix();
    Matrix4x4f view = camera->GetViewMatrix();
    
    glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, proj.GetData());
    glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, view.GetData());
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uniforms.texture, 0);
    
    for (auto* sprite : sprites) {
        if (!sprite || !sprite->IsActive()) continue;
        RenderSprite(sprite, view, proj);
    }
}

void Simple2DRenderer::RenderSprite(SpriteComponent* sprite, const Matrix4x4f& view, const Matrix4x4f& proj) {
    Texture* tex = sprite->GetTexture();
    if (!tex || !tex->IsLoaded()) return;
    
    GameObject* go = sprite->GetGameObject();
    if (!go) return;
    
    Transform2D* transform = go->GetComponentOfType<Transform2D>();
    if (!transform) return;
    
    Matrix4x4f model = transform->GetMatrix();
    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, model.GetData());
    
    const float* color = sprite->GetColor();
    glUniform4f(uniforms.color, color[0], color[1], color[2], color[3]);
    
    glBindTexture(GL_TEXTURE_2D, tex->GetOpenGLTextureID());
    glBindVertexArray(sprite->GetVAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}