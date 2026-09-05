#include "Canvas.h"

#include "UIRenderSettings.h"

Canvas::Canvas(float screenWidth, float screenHeight) 
    : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , isDirty(true) {
}

void Canvas::BuildHierarchy(){
    std::sort(uiElements.begin(), uiElements.end(),
        [](UIElement* a, UIElement* b) {
            return a->GetLayer() < b->GetLayer();
        });
}

Canvas::~Canvas() {}

void Canvas::BuildBatcheGroups() {
    groups.clear();
    if (uiElements.empty()) return;
    
    std::unordered_map<int, std::unordered_map<Texture*, std::vector<UIElement*>>> localGroups;
    
    for (auto* element : uiElements) {
        if (!element || !element->rectTransform) continue;
        
        element->UpdateGeometry();
        
        const UIRenderSettings& settings = element->GetRenderSettings();
        Texture* tex = settings.texture;
        if (!tex) continue; 
        
        int layer = element->GetLayer();
        localGroups[layer][tex].push_back(element);
    }
    
    for (auto& [layer, textureMap] : localGroups) {
        std::vector<Batch> layerBatches;
        layerBatches.reserve(textureMap.size());
        
        for (const auto& [texture, elements] : textureMap) {
            Batch batch;
            batch.texture = texture;
            
            size_t totalVertices = 0;
            size_t totalIndices = 0;
            for (auto* element : elements) {
                totalVertices += element->GetUIVertices().size();
                totalIndices += element->GetUIIndices().size();
            }
            
            batch.vertices.reserve(totalVertices);
            batch.indices.reserve(totalIndices);
            
            for (auto* element : elements) {
                const auto& verts = element->GetUIVertices();
                const auto& inds = element->GetUIIndices();
                
                batch.vertices.insert(batch.vertices.end(), verts.begin(), verts.end());
                
                unsigned int offset = static_cast<unsigned int>(batch.vertices.size() - verts.size());
                for (unsigned int idx : inds) {
                    batch.indices.push_back(offset + idx);
                }
            }
            
            if (!batch.vertices.empty()) {
                layerBatches.push_back(std::move(batch));
            }
        }
        
        if (!layerBatches.empty()) {
            groups[layer] = std::move(layerBatches);
        }
    }
}

void Canvas::RemoveUIElement(UIElement* uiElement) {
    auto it = std::find_if(uiElements.begin(), uiElements.end(),
        [uiElement](const UIElement* el) {
            return el == uiElement;
        });
    
    if (it != uiElements.end()) {
        (*it)->SetCanvas(nullptr);
        uiElements.erase(it);
        isDirty = true;
    }
}

void Canvas::Update(){
    if(!isDirty) return;

    BuildHierarchy();
    BuildBatcheGroups();

    isDirty = false;
}

void Canvas::SetScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    isDirty = true;
}