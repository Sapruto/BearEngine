#include "ModelRenderer.h"

#include "GraphicsManager.h"
#include "Transform3D.h"
#include "Camera3D.h"
#include "DirectionalLight3D.h"
#include "SimpleModelRenderer.h"
#include "RenderPipeline.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

void ModelRenderer::InitBaseRenderer(){
    RegisterModelFeatureRenderer(new SimpleModelRenderer());
    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        std::vector<ModelFeature*> features = model->GetFeatures();
        if(features.size() != 0) continue;

        groups[ModelFeatureType::Simple].push_back(model);
    }
}

void ModelRenderer::BuildGroups(){
    if (!isDirty) return;

    for(auto& [type, group] : groups){
        group.clear();
    }

    for(auto* component : rendererComponents){
        ModelComponent* model = dynamic_cast<ModelComponent*>(component);
        if(!model) continue;

        std::vector<ModelFeature*> features = model->GetFeatures();

        groups[ModelFeatureType::Simple].push_back(model);
        for(auto* feature : features){
            groups[feature->type].push_back(model);
        }
    }
    InitBaseRenderer();

    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers) {
        if (modelFeatureRenderer) modelFeatureRenderer->Init();
    }

    isDirty = false;
}

void ModelRenderer::Start() {
    if (isInitialized) return;

    if (!manager) {
        std::cout << "NO MANAGER EBANY TYPICE" << std::endl;
        return;
    }

    Camera* cam = manager->GetCamera();
    Camera3D* camera = dynamic_cast<Camera3D*>(cam);
    if (!camera) {
        std::cout << "NO CAMERA EBANY TYPICE" << std::endl;
        return;
    }

    Window* window = manager->GetWindow();
    if (!window) {
        std::cout << "NO WINDOW EBANY TYPICE" << std::endl;
        return;
    }

    uvManager = std::make_unique<UVManager>();
    uvManager->SetCamera(camera);
    
    pipeline = std::make_unique<RenderPipeline>(camera, window);
    pipeline->SetUVManager(uvManager.get());
    
    InitBaseRenderer();

    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
        if (modelFeatureRenderer) modelFeatureRenderer->Init();
    }

    isInitialized = true;
}

void ModelRenderer::Update(){
    if (!isInitialized) return;

    BuildGroups();
    
    UpdateUVs();
    
    pipeline->BeginFrame();

    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
        if(!modelFeatureRenderer || !groups.count(type)) continue;

        modelFeatureRenderer->RenderGroup(groups[type], pipeline.get());
    }
    pipeline->EndFrame();
        
    pipeline->Present();
}