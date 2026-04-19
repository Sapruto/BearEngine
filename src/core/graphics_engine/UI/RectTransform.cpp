#include "RectTransform.h"

#include "UIRect.h"

#include <glm/glm.hpp>

RectTransform::RectTransform() : 
    anchoredPosition("anchoredPosition", Vector2(0, 0)),
    sizeDelta("sizeDelta", Vector2(100, 100)),
    pivot("pivot", Vector2(0.5f, 0.5f)),
    anchorMin("anchorMin", Vector2(0.5f, 0.5f)),
    anchorMax("anchorMax", Vector2(0.5f, 0.5f)),
    rotation("rotation", 0.0f),
    parent(nullptr) {
}

void RectTransform::SetAnchors(Vector2 anchorMin, Vector2 anchorMax) {
        this->anchorMin.GetValue() = anchorMin;
        this->anchorMax.GetValue() = anchorMax;
}

UIRect RectTransform::GetScreenRect(float screenWidth, float screenHeight) const {
    UIRect parentRect;
    
    if (parent && parent != this) {
        parentRect = parent->GetScreenRect(screenWidth, screenHeight);
    } 
    else {
        parentRect = UIRect(0, 0, screenWidth, screenHeight);
    }
    
    float anchorX = parentRect.x + parentRect.width * anchorMin.GetValue().x;
    float anchorY = parentRect.y + parentRect.height * anchorMin.GetValue().y;
    
    float posX = anchorX + anchoredPosition.GetValue().x;
    float posY = anchorY + anchoredPosition.GetValue().y;
    
    float width = sizeDelta.GetValue().x;
    float height = sizeDelta.GetValue().y;
    
    if (anchorMin.GetValue().x != anchorMax.GetValue().x) {
        width = parentRect.width * (anchorMax.GetValue().x - anchorMin.GetValue().x) + sizeDelta.GetValue().x;
    }
    
    if (anchorMin.GetValue().y != anchorMax.GetValue().y) {
        height = parentRect.height * (anchorMax.GetValue().y - anchorMin.GetValue().y) + sizeDelta.GetValue().y;
    }
    
    posX -= width * pivot.GetValue().x;
    posY -= height * pivot.GetValue().y;
    
    return UIRect(posX, posY, width, height);
}