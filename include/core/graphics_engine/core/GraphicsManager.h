#pragma once

#include <vector>
#include <memory>

#include "Renderer.h"
#include "Window.h"
#include "Camera.h"

class GraphicsManager {
private:
    std::unique_ptr<Window> window;

    Camera* camera;
    
    std::vector<std::unique_ptr<Renderer>> renders;
    
public:
    GraphicsManager() = default;

    void SetCamera(Camera* c) { camera = c; }
    void SetWindow(Window* w) { window.reset(w); }

    void Start() {
        for(auto& render : renders){
            render->Start();
        }
    }
    void Update() {
        for(auto& render : renders){
            render->Update();
        }
    }
    void Destroy() {
        for(auto& render : renders){
            render->Destroy();
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

    Camera* GetCamera() { return camera; }
    Window* GetWindow() { return window.get(); }
};