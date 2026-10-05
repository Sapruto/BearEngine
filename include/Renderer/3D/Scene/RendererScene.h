#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Light3D.h"
#include "IRenderer.h"
#include "MeshShadingType.h"
#include "Shader.h"
#include "UVManager.h"
#include "GraphicsManager.h" 
#include "Camera3D.h"

#include <vector>
#include <memory>
#include <algorithm> 

class Transform3D;
class Camera3D;
class Window;
class RenderPipeline;

class RendererScene : public Renderer{
private:
    std::unordered_map<MeshShadingType, std::vector<Mesh*>> groups;
    std::unordered_map<MeshShadingType, IRenderer*> modelFeatureRenderers;

    std::unique_ptr<UVManager> uvManager;
    std::unique_ptr<RenderPipeline> pipeline;

    bool isDirty = true;
    bool isInitialized = false;

    void InitBaseRenderer();
    void BuildGroups();

    void UpdateUVs() {
        std::vector<Mesh*> allModels;
        for (auto& [type, models] : groups) {
            allModels.insert(allModels.end(), models.begin(), models.end());
        }
        
        int width = pipeline->GetWidth();
        int height = pipeline->GetHeight();

        if (width <= 0 || height <= 0) return;
        
        float aspect = (float)width / height;
        if (aspect <= 0.001f) aspect = 1.0f;
        uvManager->SetAspect(aspect);
        uvManager->UpdateUVs(allModels);
    }

public:
    RendererScene() = default;
    ~RendererScene() = default;

    void Start() override;
    void Update() override;
    void Destroy() override {
        for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
            if(modelFeatureRenderer) modelFeatureRenderer->Shutdown();
        }
    }

    template<typename T>
    T* RegisterIRenderer(T* renderer) {
        static_assert(std::is_base_of<IRenderer, T>::value, "T must be derived from IRenderer");

        modelFeatureRenderers[renderer->GetType()] = renderer;
        if (!isInitialized) renderer->Init();
        isDirty = true;
        return renderer;
    }

    template<typename T>
    void UnregisterIRenderer(T* renderer) {
        auto it = modelFeatureRenderers.find(renderer->type);
        if(it != modelFeatureRenderers.end() && it->second == renderer){
            modelFeatureRenderers.erase(it);
        }
        isDirty = true;
    }

    RenderPipeline* GetPipeline() {
        return pipeline.get();
    }

    ObjectUVData GetObjectUV(Mesh* model) {
        return uvManager->GetObjectUV(model);
    }
};