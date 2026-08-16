#pragma once

#include "Model.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "ModelFeature.h"

#include "SerializeField.h"
#include "SerializeFieldIndexable.h"

#include <string>
#include <memory>
#include <vector>

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
    std::vector<std::unique_ptr<ModelFeature>> dynamicFeature;

    unsigned int VAO = 0, VBO = 0, EBO = 0;

    void CreateBuffers();

public:
    ModelComponent(ResourceManager& resource, ModelRenderer& r, const std::string& modelPath, float colorRGB[3]);
    ModelComponent() = default;
    ~ModelComponent() override;

    void SetRenderer(ModelRenderer& r);
    void SetColor(float colorRGB[3]);
    void SetColor(const Vector3f& color) {
        this->colorRGB[0] = color.x;
        this->colorRGB[1] = color.y;
        this->colorRGB[2] = color.z;
    }
    
    Model* GetModel() const { return model; }
    const float* GetColor() const { return colorRGB; }
    const std::string& GetModelPath() const { return modelPath.GetValue(); }

    unsigned int GetVAO() const { return VAO; }
    unsigned int GetVBO() const { return VBO; }
    unsigned int GetEBO() const { return EBO; }
    void SetVBO(unsigned int vbo) { VBO = vbo; }
    void SetEBO(unsigned int ebo) { EBO = ebo; }

    void AddStaticFeature(ModelFeature* feature) {
        staticFeatures.push_back(feature);
    };

    void AddFeature(std::unique_ptr<ModelFeature> feature) {
        dynamicFeature.push_back(std::move(feature));
    }

    template<typename T>
    T* GetFeatureOfType() {
        for (auto& feature : dynamicFeature) {
            T* casted = dynamic_cast<T*>(feature.get());
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }
    
    std::vector<ModelFeature*> GetFeatures() {
        std::vector<ModelFeature*> result;
        result.reserve(dynamicFeature.size());
        for (auto& feature : dynamicFeature) {
            result.push_back(feature.get());
        }
        return result;
    }

    unsigned int GetID() const { return m_ID; }

    void Start() override;

    SERIALIZED_FIELDS(&modelPath, &staticFeatures)
};