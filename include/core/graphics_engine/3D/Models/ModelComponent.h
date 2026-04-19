#pragma once

#include "Model.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "ModelFeature.h"

#include <string>

class ModelRenderer;

class ModelComponent : public RenderComponent {
private:
    static unsigned int s_NextID;
    unsigned int m_ID;

    ResourceManager* rm{nullptr};
    Model* model{nullptr};
    ModelRenderer* renderer{nullptr}; 

    std::string modelPath; 

    float colorRGB[3]; 

    std::vector<ModelFeature*> features;

    unsigned int VAO, VBO, EBO;

    void CreateBuffers();

public:
    ModelComponent(ResourceManager& resource, ModelRenderer& r, const std::string& modelPath, float colorRGB[3]);
    ModelComponent() = default;
    ~ModelComponent() override; 

    void SetColor(float colorRGB[3]);   
    
    Model* GetModel() const { return model; }
    const float* GetColor() const { return colorRGB; }
    const std::string& GetModelPath() const { return modelPath; }

    unsigned int GetVAO() const { return VAO; } 

    void AddFeature(ModelFeature* feature) {
        features.push_back(feature);
    }

    template<typename T>
    T* GetFeatureOfType() {
        for (auto* feature : features) {
            T* casted = dynamic_cast<T*>(feature);
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }
    std::vector<ModelFeature*> GetFeatures() { return features; }

    unsigned int GetID() const { return m_ID; }
};