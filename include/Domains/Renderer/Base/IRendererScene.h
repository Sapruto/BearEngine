#pragma once

#include <vector>
#include <algorithm> 
#include "Domains/Core/DomainSystem.h"
#include "RHI/Base/Handles.h"
#include "IRenderable.h"

class GraphicsManager;

template<typename RenderableT>
    requires (std::is_base_of_v<IRenderable, RenderableT>)
class IRendererScene : public Domains::IDomainSystem {
protected:
    bool isDirty{true};

    std::vector<RenderableT*> renderables;

    RHI::Handles::RenderTargetHandle renderTarget;

public:
    IRendererScene() = default;
    ~IRendererScene() { ClearAll(); }

    void ClearAll() {
        for(auto renderable : renderables){
            UnregisterRenderable(renderable);
        }
    }

    RenderableT* RegisterRenderable(RenderableT* renderable) {
        renderables.push_back(renderable);
        isDirty = true;
        
        return renderable;
    }

    void UnregisterRenderable(RenderableT* renderable) {
        auto it = std::find(renderables.begin(), 
                        renderables.end(), renderable);
        if (it != renderables.end()) {
            renderables.erase(it);
        }
        isDirty = true;
    }

    RHI::Handles::RenderTargetHandle GetRenderTarget() const { return renderTarget; }
};