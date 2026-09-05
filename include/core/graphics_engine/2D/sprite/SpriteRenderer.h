#pragma once

#include "Renderer.h"
#include "SpriteComponent.h"
#include "Shader.h"
#include "BaseRenderer2D.h"
#include "SpriteFeatureType.h"
#include "UVManager2D.h"
#include "GraphicsManager.h"
#include "Camera2D.h"
#include "RenderPipeline2D.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

class Transform2D;
class Camera2D;
class Window;
class RenderPipeline2D;

class SpriteRenderer : public Renderer {
private:
    std::unordered_map<SpriteFeatureType, std::vector<SpriteComponent*>> groups;
    std::unordered_map<SpriteFeatureType, BaseRenderer2D*> spriteFeatureRenderers;

    std::unique_ptr<UVManager2D> uvManager;
    std::unique_ptr<RenderPipeline2D> pipeline;

    bool isInitialized = false;

    void InitBaseRenderer();
    void BuildGroups();

    void UpdateUVs() {
        std::vector<SpriteComponent*> allSprites;
        for (auto& [type, sprites] : groups) {
            allSprites.insert(allSprites.end(), sprites.begin(), sprites.end());
        }

        GraphicsManager* manager = GetManager();
        Camera2D* camera = dynamic_cast<Camera2D*>(manager->GetCamera());
        
        int width = pipeline->GetWidth();
        int height = pipeline->GetHeight();

        if (width <= 0 || height <= 0) return;
        
        float aspect = (float)width / height;
        if (aspect <= 0.001f) aspect = 1.0f;
        
        uvManager->UpdateUVs(allSprites);
    }

public:
    SpriteRenderer() = default;
    ~SpriteRenderer() = default;

    void Start() override;
    void Update() override;
    void Destroy() override {
        for(auto& [type, spriteFeatureRenderer] : spriteFeatureRenderers){
            if(spriteFeatureRenderer) spriteFeatureRenderer->Shutdown();
        }
    }

    template<typename T>
    T* RegisterSpriteFeatureRenderer(T* renderer) {
        static_assert(std::is_base_of<BaseRenderer2D, T>::value, "T must be derived from BaseRenderer2D");

        spriteFeatureRenderers[renderer->GetType()] = renderer;
        if (isInitialized) renderer->Init();
        return renderer;
    }

    template<typename T>
    void UnregisterSpriteFeatureRenderer(T* renderer) {
        auto it = spriteFeatureRenderers.find(renderer->GetType());
        if (it != spriteFeatureRenderers.end() && it->second == renderer) {
            spriteFeatureRenderers.erase(it);
        }
    }

    SpriteUVData GetSpriteUV(SpriteComponent* sprite) {
        return uvManager->GetSpriteUV(sprite);
    }
};