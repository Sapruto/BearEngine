#pragma once

#include "ModelComponent.h"
#include "ModelRenderer.h"

unsigned int ModelComponent::s_NextID = 0;

ModelComponent::ModelComponent(ResourceManager& resource, ModelRenderer& r, const std::string& modelPath, float colorRGB[3]) 
    : rm(&resource), renderer(&r), m_ID(s_NextID++)
{
    SetColor(colorRGB);

    this->modelPath.GetValue() = modelPath;

    rm->LoadResource(this->modelPath.GetValue(), ResourceType::Model);
    model = rm->GetResourceAs<Model>(this->modelPath.GetValue());

    if (model && model->IsLoaded()) {
        CreateBuffers();
    }
    
    if (renderer) {
        renderer->RegisterRenderComponent(this);
    }
}

ModelComponent::~ModelComponent() {
    if (renderer) {
        renderer->UnRegisterRenderComponent(this);
    }
}

void ModelComponent::CreateBuffers() {
    if (glfwGetCurrentContext() == nullptr) {
        std::cout << "ERROR: No OpenGL context!" << std::endl;
        return;
    }

    const auto& vertices = model->GetVertices();
    const auto& indices = model->GetIndices();

    if (vertices.empty() || indices.empty()) {
        std::cout << "ERROR: No vertices or indices!" << std::endl;
        return;
    }
    
    std::cout << "Creating buffers: " << vertices.size() << " vertices, " << indices.size() << " indices" << std::endl;
    
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = 0;
        VBO = 0;
        EBO = 0;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    std::cout << "Generated: VAO=" << VAO << ", VBO=" << VBO << ", EBO=" << EBO << std::endl;
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                 vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, Normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error in CreateBuffers: " << err << std::endl;
    }
    
    glBindVertexArray(0);
    
    std::cout << "Buffers created successfully" << std::endl;
}

void ModelComponent::SetRenderer(ModelRenderer& r) {
    renderer = &r;
}

void ModelComponent::SetColor(float colorRGB[3]){
    this->colorRGB[0] = colorRGB[0];
    this->colorRGB[1] = colorRGB[1];
    this->colorRGB[2] = colorRGB[2];
}

void ModelComponent::Start() {
    if(!gameObject) return;

    Scene* scene = gameObject->GetScene();
    if(!scene) return;

    if (!rm) rm = scene->GetResourceManager();

    if (!rm->GetResourceAs<Model>(modelPath.GetValue())) rm->LoadResource(modelPath.GetValue(), ResourceType::Model);
    if (!model) model = rm->GetResourceAs<Model>(modelPath.GetValue());

    if (model && model->IsLoaded()) {
        if (VAO == 0) CreateBuffers();
    }
    
    if (renderer) {
        renderer->RegisterRenderComponent(this);
    }
}