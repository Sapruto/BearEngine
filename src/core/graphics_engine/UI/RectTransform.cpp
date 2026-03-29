#include "RectTransform.h"

#include "UIRect.h"

#include <glm/glm.hpp>

void RectTransform::SetAnchors(Vector2 anchorMin, Vector2 anchorMax) {
        this->anchorMin = anchorMin;
        this->anchorMax = anchorMax;
}

UIRect RectTransform::GetScreenRect(float screenWidth, float screenHeight) const {
    UIRect parentRect;
    
    if (parent && parent != this) {
        parentRect = parent->GetScreenRect(screenWidth, screenHeight);
    } 
    else {
        parentRect = UIRect(0, 0, screenWidth, screenHeight);
    }
    
    float anchorX = parentRect.x + parentRect.width * anchorMin.x;
    float anchorY = parentRect.y + parentRect.height * anchorMin.y;
    
    float posX = anchorX + anchoredPosition.x;
    float posY = anchorY + anchoredPosition.y;
    
    float width = sizeDelta.x;
    float height = sizeDelta.y;
    
    if (anchorMin.x != anchorMax.x) {
        width = parentRect.width * (anchorMax.x - anchorMin.x) + sizeDelta.x;
    }
    
    if (anchorMin.y != anchorMax.y) {
        height = parentRect.height * (anchorMax.y - anchorMin.y) + sizeDelta.y;
    }
    
    posX -= width * pivot.x;
    posY -= height * pivot.y;
    
    return UIRect(posX, posY, width, height);
}