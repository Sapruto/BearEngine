#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstring>

#include "RenderComponent.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "SpriteFeature.h"
#include "SerializeField.h"
#include "SerializeFieldIndexable.h"

class SpriteRenderer;

class SpriteComponent : public RenderComponent {
private:
    static unsigned int s_NextID;
    unsigned int m_ID;
    
    ResourceManager* rm = nullptr;
    Texture* texture = nullptr;
    
    FIELD(std::string, texturePath);
    FIELD(float, width);
    FIELD(float, height);
    float color[4];
    
    FIELD_INDEXABLE(std::vector<SpriteFeature*>, staticFeatures);
    std::vector<std::unique_ptr<SpriteFeature>> dynamicFeatures;
    
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    bool buffersCreated = false;
    
    void CreateBuffers();
    void LoadTexture();
    
public:
    SpriteComponent();
    SpriteComponent(ResourceManager& resource, const std::string& texturePath);
    ~SpriteComponent() override;
    
    void Start() override;
    
    void SetTexture(const std::string& path);
    Texture* GetTexture() const { return texture; }
    
    void SetSize(float w, float h);
    float GetWidth() const { return width.GetValue(); }
    float GetHeight() const { return height.GetValue(); }
    
    void SetColor(const float newColor[4]) {
        memcpy(color, newColor, sizeof(color));
    }

    const float* GetColor() const {
        return color;
    }
    void GetColor(float out[4]) const {
        memcpy(out, color, sizeof(color));
    }
    
    void AddFeature(std::unique_ptr<SpriteFeature> feature);

    template<typename T>
    T* GetFeatureOfType() {
        for (auto& feature : dynamicFeatures) {
            T* casted = dynamic_cast<T*>(feature.get());
            if (casted) return casted;
        }
        return nullptr;
    }
    std::vector<SpriteFeature*> GetFeatures();
    
    unsigned int GetID() const { return m_ID; }
    unsigned int GetVAO() const { return VAO; }
    bool HasBuffers() const { return buffersCreated; }
    
    SERIALIZED_FIELDS(&texturePath, &width, &height)
};