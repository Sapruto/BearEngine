#include "SpriteRenderer.h"

#include "GraphicsManager.h"
#include "Transform2D.h"
#include "Camera2D.h"
#include "Window.h"
#include "Simple2DRenderer.h"
#include "Transparent2DRenderer.h"

void SpriteRenderer::InitBaseRenderer() {
    RegisterSpriteFeatureRenderer(new Simple2DRenderer());
    
    for (auto* component : rendererComponents) {
        SpriteComponent* sprite = dynamic_cast<SpriteComponent*>(component);
        if (!sprite) continue;

        std::vector<SpriteFeature*> features = sprite->GetFeatures();
        if (features.empty()) {
            groups[SpriteFeatureType::Simple].push_back(sprite);
        }
    }
}

void SpriteRenderer::BuildGroups() {
    if (!isDirty) return;

    for (auto& [type, group] : groups) {
        group.clear();
    }

    for (auto* component : rendererComponents) {
        SpriteComponent* sprite = dynamic_cast<SpriteComponent*>(component);
        if (!sprite) continue;

        std::vector<SpriteFeature*> features = sprite->GetFeatures();
        if (features.empty()) {
            groups[SpriteFeatureType::Simple].push_back(sprite);
        } else {
            for (auto* feature : features) {
                groups[feature->type].push_back(sprite);
            }
        }
    }
    
    InitBaseRenderer();
    isDirty = false;
}

void SpriteRenderer::Start() {
    if (!manager) {
        std::cout << "NO MANAGER AND YOU ARE F*CKING STUPED MONKEY" << std::endl;
        return;
    }
    Camera2D* camera = dynamic_cast<Camera2D*>(manager->GetCamera());
    Window* window = manager->GetWindow();
    
    uvManager = std::make_unique<UVManager2D>();
    pipeline = std::make_unique<RenderPipeline2D>(camera, window);
    
    pipeline->SetUVManager(uvManager.get());
    
    InitBaseRenderer();
}

void SpriteRenderer::Update() {
    BuildGroups();
    
    UpdateUVs();
    
    pipeline->BeginFrame();
    
    for (auto& [type, renderer] : spriteFeatureRenderers) {
        if (!renderer || !groups.count(type)) continue;
        renderer->RenderGroup(groups[type], pipeline.get());
    }
    
    pipeline->EndFrame();
    pipeline->Present();
}