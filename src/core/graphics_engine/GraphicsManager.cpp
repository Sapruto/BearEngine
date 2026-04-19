#include "GraphicsManager.h"

void GraphicsManager::RemoveRender(Renderer* render){
    auto it = std::find_if(renders.begin(), renders.end(),
        [render](const std::unique_ptr<Renderer>& re) {
            return re.get() == render;
        });
    
    if (it != renders.end()) {
        (*it)->ClearAll();
        renders.erase(it);
    }
}

void GraphicsManager::ApplyRenderSettingsToCamera(bool applyOffset) {

}