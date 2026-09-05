#include "SpriteComponent.h"

#include "ResourceManager.h"
#include "Transform2D.h"
#include "GameObject.h"
#include "Scene.h"
#include <glad/glad.h>
#include <cstring>

unsigned int SpriteComponent::s_NextID = 0;

SpriteComponent::SpriteComponent() {
    m_ID = s_NextID++;
    width.GetValue() = 100.0f;
    height.GetValue() = 100.0f;
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;
}

SpriteComponent::SpriteComponent(ResourceManager& resource, const std::string& path)
    : rm(&resource) {
    m_ID = s_NextID++;
    texturePath.GetValue() = path;
    width.GetValue() = 100.0f;
    height.GetValue() = 100.0f;
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;
    
    LoadTexture();
    CreateBuffers();
}

SpriteComponent::~SpriteComponent() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void SpriteComponent::Start() {
    if (!gameObject) return;
    
    Scene* scene = gameObject->GetScene();
    if (!scene) return;
    
    if (!rm) rm = scene->GetResourceManager();
    
    if (!rm->GetResource(texturePath.GetValue())) {
        rm->LoadResource(texturePath.GetValue(), ResourceType::Texture);
    }
    texture = static_cast<Texture*>(rm->GetResource(texturePath.GetValue()));
    
    if (texture && texture->IsLoaded()) {
        if (!buffersCreated) CreateBuffers();
    }
}

void SpriteComponent::LoadTexture() {
    if (!rm || texturePath.GetValue().empty()) return;
    
    if (!rm->GetResource(texturePath.GetValue())) {
        rm->LoadResource(texturePath.GetValue(), ResourceType::Texture);
    }
    texture = static_cast<Texture*>(rm->GetResource(texturePath.GetValue()));
}

void SpriteComponent::CreateBuffers() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    
    float vertices[] = {
        -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.0f, 0.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    buffersCreated = true;
}

void SpriteComponent::SetTexture(const std::string& path) {
    texturePath.GetValue() = path;
    LoadTexture();
}

void SpriteComponent::SetSize(float w, float h) {
    width.GetValue() = w;
    height.GetValue() = h;
}

void SpriteComponent::AddFeature(std::unique_ptr<SpriteFeature> feature) {
    dynamicFeatures.push_back(std::move(feature));
}

std::vector<SpriteFeature*> SpriteComponent::GetFeatures() {
    std::vector<SpriteFeature*> result;
    result.reserve(dynamicFeatures.size());
    for (auto& feature : dynamicFeatures) {
        result.push_back(feature.get());
    }
    return result;
}