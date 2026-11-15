#pragma once

#include <vector>
#include <algorithm> 
#include "Domains/Core/DomainSystem.h"
#include "RHI/Base/Handles.h"
#include "IRenderable.h"

class GraphicsManager;

class IRendererScene : public Domains::IDomainSystem {
protected:
    bool isDirty{true};

    std::vector<IRenderable*> renderables;

    RHI::Handles::RenderTargetHandle renderTarget;

public:
    IRendererScene() = default;
    ~IRendererScene() { ClearAll(); }

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

    RHI::Handles::RenderTargetHandle GetRenderTarget() const { return renderTarget; }
};