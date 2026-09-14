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

#include <chrono>
#include <iostream>

void ModelRenderer::Update() {
    if (!isInitialized) return;

    static auto t_last = std::chrono::high_resolution_clock::now();
    static double acc_build = 0, acc_render = 0, acc_present = 0;
    static int frames = 0;

    auto t0 = std::chrono::high_resolution_clock::now();
    BuildGroups();
    auto t1 = std::chrono::high_resolution_clock::now();

    for(auto& [type, modelFeatureRenderer] : modelFeatureRenderers){
        if(!modelFeatureRenderer || !groups.count(type)) continue;
        pipeline->BeginCurrentRender();
        modelFeatureRenderer->RenderGroup(groups[type], pipeline.get());
        pipeline->EndCurrentRender();
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    pipeline->Present();
    auto t3 = std::chrono::high_resolution_clock::now();

    acc_build += std::chrono::duration<double, std::milli>(t1 - t0).count();
    acc_render += std::chrono::duration<double, std::milli>(t2 - t1).count();
    acc_present += std::chrono::duration<double, std::milli>(t3 - t2).count();
    frames++;

    if (std::chrono::duration<double>(t3 - t_last).count() > 1.0) {
        std::cout << "BuildGroups: " << acc_build / frames << " ms | "
                  << "Render: " << acc_render / frames << " ms | "
                  << "Present: " << acc_present / frames << " ms | "
                  << "FPS: " << frames << std::endl;
        acc_build = acc_render = acc_present = 0;
        frames = 0;
        t_last = t3;
    }
}