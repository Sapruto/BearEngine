#pragma once

#include <vector>
#include <memory>

#include "Renderer.h"
#include "DrawingWindow.h"
#include "Camera.h"   
#include "RenderSettings.h" 

class GraphicsManager {
private:
    std::unique_ptr<DrawingManager> window;
    std::unique_ptr<Camera2D> camera;
    std::unique_ptr<RenderSettings> renderSettings; 
    
    std::vector<std::unique_ptr<Renderer>> renders;
    
public:
    GraphicsManager() {
        renderSettings = std::make_unique<RenderSettings>();
    }

    void SetCamera(Camera2D* c) { camera.reset(c); }
    void SetWindow(DrawingManager* w) { window.reset(w); }
    void SetRenderSettings(RenderSettings* settings) { renderSettings.reset(settings); }

    void Update(){
        for(auto& render : renders){
            render->Update();
        }
    }

    template<typename T, typename... Args>
    T* AddRender(Args&&... args) {
        static_assert(std::is_base_of<Renderer, T>::value, "T must be Renderer");
        auto render = std::make_unique<T>(std::forward<Args>(args)...);
        render->SetManager(this);
        T* ptr = render.get();
        renders.push_back(std::move(render));
        return ptr;
    }
    
    void RemoveRender(Renderer* render);

    template<typename T>
    T* GetRenderer() {
        for (auto& render : renders) {
            T* ptr = dynamic_cast<T*>(render.get());
            if (ptr) return ptr;
        }
        return nullptr;
    }

    void ApplyRenderSettingsToCamera(bool applyOffset = false);

    Camera2D* GetCamera() { return camera.get(); }
    DrawingManager* GetWindow() { return window.get(); }
    RenderSettings* GetRenderSettings() { return renderSettings.get(); }
};