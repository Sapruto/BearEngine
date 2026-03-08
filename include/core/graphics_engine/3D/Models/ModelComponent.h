#pragma once

#include "Model.h"
#include "RenderComponent.h"
#include "ResourceManager.h"

#include <string>

class ModelRenderer;

class ModelComponent : public RenderComponent {
private:
    ResourceManager& rm;
    Model* model;
    ModelRenderer* renderer; 

    std::string modelPath; 

    float colorRGB[3]; 

    unsigned int VAO, VBO, EBO;

    void CreateBuffers();

public:
    ModelComponent(ResourceManager& rm, ModelRenderer& r, const std::string& modelPath, float colorRGB[3]);
    ~ModelComponent() override; 

    void SetColor(float colorRGB[3]);   
    
    Model* GetModel() const { return model; }
    const float* GetColor() const { return colorRGB; }
    const std::string& GetModelPath() const { return modelPath; }

    unsigned int GetVAO() const { return VAO; } 

    void Render(const glm::vec2& position, float rotation) override {  }
    bool IsVisible() const override { return true; }
    int GetLayer() const override { return 0; }
};