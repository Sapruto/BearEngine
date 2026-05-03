#pragma once

#include "Model.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "ModelFeature.h"

#include "SerializeField.h"
#include "SerializeFieldIndexable.h"

#include <string>

class ModelRenderer;

class ModelComponent : public RenderComponent {
private:
    static unsigned int s_NextID;
    unsigned int m_ID;

    ResourceManager* rm{nullptr};
    Model* model{nullptr};
    ModelRenderer* renderer{nullptr}; 

    FIELD(std::string, modelPath); 

    float colorRGB[3]; 

    FIELD_INDEXABLE(std::vector<ModelFeature*>, staticFeatures);
    std::vector<ModelFeature*> dynamicFeature;

    unsigned int VAO, VBO, EBO;

    void CreateBuffers();

public:
    ModelComponent(ResourceManager& resource, ModelRenderer& r, const std::string& modelPath, float colorRGB[3]);
    ModelComponent() = default;
    ~ModelComponent() override; 

    void SetRenderer(ModelRenderer& r);
    void SetColor(float colorRGB[3]);   
    
    Model* GetModel() const { return model; }
    const float* GetColor() const { return colorRGB; }
    const std::string& GetModelPath() const { return modelPath.GetValue(); }

    unsigned int GetVAO() const { return VAO; } 

    void AddStaticFeature(ModelFeature* feature) {
        staticFeatures.push_back(feature);
    };

    void AddFeature(ModelFeature* feature) {
        dynamicFeature.push_back(feature);
    }

    template<typename T>
    T* GetFeatureOfType() {
        for (auto* feature : dynamicFeature) {
            T* casted = dynamic_cast<T*>(feature);
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }
    std::vector<ModelFeature*> GetFeatures() { return dynamicFeature; }

    unsigned int GetID() const { return m_ID; }

    void Start() override;

    SERIALIZED_FIELDS(&modelPath, &staticFeatures)
};