#pragma once

#include "Renderer.h"
#include "ModelComponent.h"
#include "Shader.h"
#include "Light3D.h"
#include "ModelFeatureRenderer.h"
#include "ModelFeatureType.h"
#include "Shader.h"
#include "UVManager.h"
#include "GraphicsManager.h" 
#include "Camera3D.h"

#include <vector>
#include <memory>
#include <algorithm> 

class Transform3D;
class Camera3D;

class ModelRenderer : public Renderer{
private:
    std::vector<std::unique_ptr<Light3D>> lights; 

    std::unordered_map<ModelFeatureType, std::vector<ModelComponent*>> groups;
    std::unordered_map<ModelFeatureType, ModelFeatureRenderer*> modelFeatureRenderers;

    GLuint framebuffers[2];
    GLuint textures[2];
    GLuint depthBuffers[2];
    
    int currentRead = 0;
    int currentWrite = 1;

    int textureWidth, textureHeight;

    GLuint quadVAO;
    GLuint quadVBO;

    Shader m_Shader;

    UVManager m_UVManager;

    void InitQuad();
    void CreateFramebuffers(int width, int height);
    void RenderResultTexture(GLuint texture);

    void InitBaseRenderer();
    void BuildGroups();

    void UpdateUVs() {
        std::vector<ModelComponent*> allModels;
        for (auto& [type, models] : groups) {
            allModels.insert(allModels.end(), models.begin(), models.end());
        }
        
        GraphicsManager* manager = GetManager();
        Camera3D* camera = dynamic_cast<Camera3D*>(manager->GetCamera());
        int width = GetTextureWidth();
        int height = GetTextureHeight();
        
        m_UVManager.UpdateUVs(allModels, camera, (float)width/height);
    }

public:
    ModelRenderer();
    ~ModelRenderer();

    void Update() override;

    template<typename T, typename... Args>
    T* AddLight(Args&&... args) {
        static_assert(std::is_base_of<Light3D, T>::value, "T must be derived from Light3D");
        
        auto light = std::make_unique<T>(std::forward<Args>(args)...);
        
        T* ptr = light.get();
        
        lights.push_back(std::move(light));
        
        return ptr;
    }
    
    void RemoveLight(Light3D* light);

    template<typename T>
    T* RegisterModelFeatureRenderer(T* renderer) {
        static_assert(std::is_base_of<ModelFeatureRenderer, T>::value, "T must be derived from ModelFeatureRenderer");

        modelFeatureRenderers[renderer->type] = renderer;
        return renderer;
    }

    template<typename T>
    void UnregisterModelFeatureRenderer(T* renderer) {
        auto it = modelFeatureRenderers.find(renderer->type);
        if(it != modelFeatureRenderers.end() && it->second == renderer){
            modelFeatureRenderers.erase(it);
        }
    }

    std::vector<Light3D*> GetLights() { 
        std::vector<Light3D*> result;
        for(auto& light : lights) result.push_back(light.get());
        return result;
    }

    GLuint GetReadTexture() const { return textures[currentRead]; }
    GLuint GetWriteTexture() const { return textures[currentWrite]; }

    GLuint GetReadDepthTexture() const { return depthBuffers[currentRead]; }

    int GetTextureWidth() const { return textureWidth; }
    int GetTextureHeight() const { return textureHeight; }

    ObjectUVData GetObjectUV(ModelComponent* model) {
        return m_UVManager.GetObjectUV(model);
    }
};