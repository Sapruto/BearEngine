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
    if (!camera || !renderSettings) return;
    
    float currentZoom = camera->GetZoom();
    camera->SetZoom(currentZoom * renderSettings->GetGlobalScale());
    
    if (applyOffset) {
        glm::vec2 currentPos = camera->GetPosition();
        camera->SetPosition(currentPos + renderSettings->GetOffset());
    }
}