#include "ModelRenderer.h"
#include "GraphicsManager.h"
#include "Transform3D.h"
#include "Camera3D.h"
#include "DirectionalLight3D.h"
#include "SimpleModelRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

ModelRenderer::ModelRenderer(){
    InitBaseRenderer();
}

ModelRenderer::~ModelRenderer(){

}

void ModelRenderer::InitBaseRenderer(){
    RegisterModelFeatureRenderer(new SimpleModelRenderer());
    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        groups[ModelFeatureType::Simple].push_back(model);
    }
}

void ModelRenderer::BuildGroups(){
    if(!isDirty) return;

    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        std::vector<ModelFeature*> features = model->GetFeatures();
        for(auto* feature : features){
            groups[feature->type].push_back(model);
        }
    }
    InitBaseRenderer();
    isDirty = false;
}

void ModelRenderer::Update(){
    BuildGroups();
    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
        if(modelFeatureRenderer && groups.count(type)){
            modelFeatureRenderer->RenderGroup(groups[type], this);
        }
    }
}

void ModelRenderer::RemoveLight(Light3D* light) {
    auto it = std::find_if(lights.begin(), lights.end(),
        [light](const std::unique_ptr<Light3D>& li) {
            return li.get() == light;
        });
    
    if (it != lights.end()) {
        lights.erase(it);
    }
}