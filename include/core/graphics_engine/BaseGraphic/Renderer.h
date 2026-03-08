#pragma once

#include <vector>
#include <algorithm> 
#include "RenderComponent.h"

class GraphicsManager;

class Renderer{
protected:
    std::vector<RenderComponent*> rendererComponents;

    GraphicsManager* manager;

public:
    Renderer() = default;
    ~Renderer() { ClearAll(); }

    virtual void Update() = 0;

    void SetManager(GraphicsManager* mgr) {
        manager = mgr;
    }

    void ClearAll(){
        this->manager = nullptr;
        
        for(auto comp : rendererComponents){
            UnRegisterRenderComponent(comp);
        }
    }

    template<typename T>
    T* RegisterRenderComponent(T* component) {
        static_assert(std::is_base_of<RenderComponent, T>::value, "T must be derived from Component");

        rendererComponents.push_back(component);
        
        return component;
    }

    template<typename T>
    void UnRegisterRenderComponent(T* component) {
        auto it = std::find(rendererComponents.begin(), 
                        rendererComponents.end(), component);
        if (it != rendererComponents.end()) {
            rendererComponents.erase(it);
        }
    }
};