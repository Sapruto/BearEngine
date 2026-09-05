#include "Transparent2DRenderer.h"

#include "SpriteComponent.h"
#include "RenderPipeline2D.h"
#include "Camera2D.h"
#include "Transform2D.h"
#include "TransparentFeature2D.h"
#include <glad/glad.h>
#include <algorithm>

Transparent2DRenderer::Transparent2DRenderer()
    : shader("include/core/graphics_engine/2D/Shaders/SpriteVertex.glsl",
             "include/core/graphics_engine/2D/Shaders/SpriteFragment.glsl") {
    type = SpriteFeatureType::Transparent;
}

void Transparent2DRenderer::Init() {
    if (isInitialized) return;
    shader.Bind();
    CacheUniformLocations();
    isInitialized = true;
}

void Transparent2DRenderer::Shutdown() {
    isInitialized = false;
}

void Transparent2DRenderer::CacheUniformLocations() {
    uniforms.projection = glGetUniformLocation(shader.GetID(), "projection");
    uniforms.view = glGetUniformLocation(shader.GetID(), "view");
    uniforms.model = glGetUniformLocation(shader.GetID(), "model");
    uniforms.texture = glGetUniformLocation(shader.GetID(), "texture1");
    uniforms.color = glGetUniformLocation(shader.GetID(), "spriteColor");
}

std::vector<SpriteComponent*> Transparent2DRenderer::SortByDepth(const std::vector<SpriteComponent*>& sprites, Camera2D* camera) {
    std::vector<SpriteComponent*> sorted = sprites;
    
    std::sort(sorted.begin(), sorted.end(), 
        [camera](SpriteComponent* a, SpriteComponent* b) {
            if (!a || !b) return false;
            
            GameObject* goA = a->GetGameObject();
            GameObject* goB = b->GetGameObject();
            if (!goA || !goB) return false;

            Transform2D* ta = goA->GetComponentOfType<Transform2D>();
            Transform2D* tb = goB->GetComponentOfType<Transform2D>();
            if (!ta || !tb) return false;
            
            if (!ta || !tb) return false;
            
            Vector2f posA = ta->GetGlobalPosition();
            Vector2f posB = tb->GetGlobalPosition();
            Vector2f camPos = camera->GetTransform()->GetGlobalPosition();
            
            float distA = posA.distanceTo(camPos);
            float distB = posB.distanceTo(camPos);
            
            return distA > distB;
        });
    
    return sorted;
}

void Transparent2DRenderer::RenderGroup(const std::vector<SpriteComponent*>& sprites, RenderPipeline2D* pipeline) {
    if (!pipeline || !isInitialized || sprites.empty()) return;
    
    Camera2D* camera = pipeline->GetCamera();
    if (!camera) return;
    
    std::vector<SpriteComponent*> sorted = SortByDepth(sprites, camera);
    
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
    
    for (auto* sprite : sorted) {
        if (!sprite || !sprite->IsActive()) continue;
        
        TransparentFeature2D* feature = sprite->GetFeatureOfType<TransparentFeature2D>();
        if (!feature) continue;
        
        RenderSprite(sprite, view, proj);
    }
}

void Transparent2DRenderer::RenderSprite(SpriteComponent* sprite, const Matrix4x4f& view, const Matrix4x4f& proj) {
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