#pragma once

#include "Model.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "MeshShading.h"

#include "Systems/Serialization/SerializeField.h"
#include "Systems/Serialization/SerializeFieldIndexable.h"

#include "Vector/Vector3.h"

#include <atomic>
#include <string>
#include <memory>
#include <vector>

class RendererScene;

class Mesh : public RenderComponent {
private:
    std::atomic<unsigned> Mesh::nextID = 0;
    unsigned int id;

    ResourceManager* rm{nullptr};
    Model* model{nullptr};

    FIELD(std::string, modelPath);
    FIELD(Vector3f, colorRGB);

    FIELD_INDEXABLE(std::vector<MeshShading*>, fieldFeatures);
    std::vector<std::unique_ptr<MeshShading>> features;

public:
    Mesh(ResourceManager& resource, const std::string& modelPath, Vector3f colorRGB);
    Mesh() : id(nextID.fetch_add(1)) = default;
    ~Mesh() override;

    void SetColor(const Vector3f& color) {
        this->colorRGB.GetValue() = color;
    }
    
    Model* GetModel() const { return model; }
    Vector3f GetColor() const { return colorRGB.GetValue(); }
    const std::string& GetModelPath() const { return modelPath.GetValue(); }

    void AddStaticFeature(MeshShading* feature) {
        fieldFeatures.push_back(feature);
    };

    void AddFeature(std::unique_ptr<MeshShading> feature) {
        features.push_back(std::move(feature));
    }

    template<typename T>
    T* GetFeatureOfType() {
        for (auto& feature : features) {
            T* casted = dynamic_cast<T*>(feature.get());
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }
    
    std::vector<MeshShading*> GetFeatures() {
        std::vector<MeshShading*> result;
        result.reserve(features.size());
        for (auto& feature : features) {
            result.push_back(feature.get());
        }
        return result;
    }

    unsigned int GetID() const { return id; }

    void Start() override;

    SERIALIZED_FIELDS(&modelPath, &fieldFeatures)
};