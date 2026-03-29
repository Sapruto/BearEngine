#pragma once

#include "Component.h"
#include "Vector2.h"
#include "UIRect.h"

class RectTransform : public Component {
private:
    Vector2 anchoredPosition; 
    Vector2 sizeDelta;     
    Vector2 pivot;          
    Vector2 anchorMin;        
    Vector2 anchorMax;   
    float rotation;
    
    RectTransform* parent;  
    
public:
    RectTransform() : anchoredPosition(0, 0), sizeDelta(100, 100),
                      pivot(0.5f, 0.5f), anchorMin(0.5f, 0.5f), 
                      anchorMax(0.5f, 0.5f), rotation(0), parent(nullptr) {}
    
    Vector2 GetAnchoredPosition() const { return anchoredPosition; }
    Vector2 GetSizeDelta() const { return sizeDelta; }
    Vector2 GetPivot() const { return pivot; }
    Vector2 GetAnchorMin() const { return anchorMin; }
    Vector2 GetAnchorMax() const { return anchorMax; }
    float GetRotation() const { return rotation; }
    RectTransform* GetParent() const { return parent; }
    
    void SetAnchoredPosition(const Vector2& pos) { anchoredPosition = pos; }
    void SetSizeDelta(const Vector2& size) { sizeDelta = size; }
    void SetPivot(const Vector2& newPivot) { pivot = newPivot; }
    void SetRotation(float rot) { rotation = rot; }
    void SetParent(RectTransform* newParent) { parent = newParent; }
    
    void SetAnchors(Vector2 anchorMin, Vector2 anchorMax);
    void SetAnchors(const Vector2& min, const Vector2& max) {
        anchorMin = min;
        anchorMax = max;
    }

    void SetAnchorMin(const Vector2& min) { anchorMin = min; }
    void SetAnchorMax(const Vector2& max) { anchorMax = max; }
    
    void SetSizeAbsolute(float width, float height) {
        sizeDelta = Vector2(width, height);
    }
    
    UIRect GetScreenRect(float screenWidth, float screenHeight) const;
    
    Vector2 GetScreenPosition(float screenWidth, float screenHeight) const {
        UIRect rect = GetScreenRect(screenWidth, screenHeight);
        return Vector2(rect.x, rect.y);
    }
    
    Vector2 GetScreenSize(float screenWidth, float screenHeight) const {
        UIRect rect = GetScreenRect(screenWidth, screenHeight);
        return Vector2(rect.width, rect.height);
    }
};