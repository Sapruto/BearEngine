#pragma once

#include <vector>
#include <algorithm> 
#include "Renderer/Base/IRenderable.h"

class GraphicsManager;

class IRendererScene {
protected:
    bool isDirty{true};

    std::vector<IRenderable*> renderables;

public:
    IRendererScene() = default;
    ~IRendererScene() { ClearAll(); }

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;

    void ClearAll() {
        for(auto renderable : renderables){
            UnregisterRenderable(renderable);
        }
    }

    template<typename T>
    T* RegisterRenderable(T* renderable) {
        static_assert(std::is_base_of<IRenderable, T>::value, "T must be derived from Component");

        renderables.push_back(renderable);
        isDirty = true;
        
        return renderable;
    }

    template<typename T>
    void UnregisterRenderable(T* renderable) {
        auto it = std::find(renderables.begin(), 
                        renderables.end(), renderable);
        if (it != renderables.end()) {
            renderables.erase(it);
        }
        isDirty = true;
    }
};