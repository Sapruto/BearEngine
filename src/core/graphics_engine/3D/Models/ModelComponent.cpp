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

void ModelComponent::CreateBuffers(){
    const auto& vertices = model->GetVertices();
    const auto& indices = model->GetIndices();

    std::cout << "Creating buffers: " << vertices.size() << " vertices, " << indices.size() << " indices" << std::endl;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                 vertices.data(), GL_DYNAMIC_DRAW);
    
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
    
    glBindVertexArray(0);
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

    Scene* scene = const_cast<Scene*>(gameObject->GetScene());
    if(!scene) return;

    if (!rm) rm = scene->GetResourceManager();

    rm->LoadResource(modelPath.GetValue(), ResourceType::Model);
    if (!model) model = rm->GetResourceAs<Model>(modelPath.GetValue());

    if (model && model->IsLoaded()) {
        CreateBuffers();
    }
    
    if (renderer) {
        renderer->RegisterRenderComponent(this);
    }
}