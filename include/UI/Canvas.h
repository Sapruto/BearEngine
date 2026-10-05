#pragma once

#include "RenderComponent.h"
#include "UIElement.h"

#include <vector>
#include <unordered_map>
#include <algorithm>

#include "UIVertex.h"

class Canvas : public RenderComponent{
private:
    struct Batch {
        Texture* texture;
        std::vector<UIVertex> vertices;
        std::vector<unsigned int> indices;
    };

    std::vector<UIElement*> uiElements;

    std::unordered_map<int, std::vector<Batch>> groups;

    float screenWidth;
    float screenHeight;
    bool isDirty{true};

    void BuildHierarchy();
    void BuildBatcheGroups(); 

public:
    Canvas() = default;
    Canvas(float screenWidth, float screenHeight);
    ~Canvas();

    void Update();
    void SetScreenSize(float width, float height);
    
    const std::unordered_map<int, std::vector<Batch>>& GetGroups() const { return groups; }
    
    void RemoveUIElement(UIElement* uiElement);

    void AddUIElement(UIElement* uiElement){
        uiElements.push_back(uiElement);
        uiElement->SetCanvas(this);
        isDirty = true;
    }

    template<typename T, typename... Args>
    T* AddUIElement(Args&&... args) {
        T* element = new T(std::forward<Args>(args)...);
        uiElements.push_back(element);
        element->SetCanvas(this);
        isDirty = true;
        return element;
    }

    float GetScreenWidth() const { return screenWidth; }
    float GetScreenHeight() const { return screenHeight; }

    UIElement* GetOverlayElement(const Vector2f& pos){
        for(int i = uiElements.size() - 1; i >= 0; i--){
            UIElement* element = uiElements[i];
            if(!element || !element->rectTransform) continue;
            
            if(!element->GetVisible()) continue;

            UIRect rect = element->rectTransform->GetScreenRect(screenWidth, screenHeight);
            
            if(pos.x >= rect.x && pos.x <= rect.x + rect.width &&
                pos.y >= rect.y && pos.y <= rect.y + rect.height){
                return element;
            }
        }
        return nullptr;
    }
};