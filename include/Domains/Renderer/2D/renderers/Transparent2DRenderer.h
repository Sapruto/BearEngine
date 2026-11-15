#pragma once

#include "BaseRenderer2D.h"
#include "Shader.h"
#include "SpriteFeatureType.h"

class SpriteComponent;
class RenderPipeline2D;
class Camera2D;

class Transparent2DRenderer : public BaseRenderer2D {
private:
    Shader shader;
    
    struct UniformLocations {
        int projection = -1;
        int view = -1;
        int model = -1;
        int texture = -1;
        int color = -1;
    } uniforms;
    
    void CacheUniformLocations();
    void RenderSprite(SpriteComponent* sprite, const Matrix4x4f& view, const Matrix4x4f& proj);
    std::vector<SpriteComponent*> SortByDepth(const std::vector<SpriteComponent*>& sprites, Camera2D* camera);
    
public:
    Transparent2DRenderer();
    
    void Init() override;
    void Shutdown() override;
    void RenderGroup(const std::vector<SpriteComponent*>& sprites, RenderPipeline2D* pipeline) override;
};