#pragma once

#include "Component.h"
#include "Vector2.h"
#include "UIRect.h"
#include "SerializeField.h"

class RectTransform : public Component {
private:
    FIELD(Vector2, anchoredPosition);
    FIELD(Vector2, sizeDelta);
    FIELD(Vector2, pivot);
    FIELD(Vector2, anchorMin);   
    FIELD(Vector2, anchorMax);
    FIELD(float, rotation);
    
    RectTransform* parent;  
    
public:
    RectTransform();
    
    Vector2 GetAnchoredPosition() const { return anchoredPosition.GetValue(); }
    Vector2 GetSizeDelta() const { return sizeDelta.GetValue(); }
    Vector2 GetPivot() const { return pivot.GetValue(); }
    Vector2 GetAnchorMin() const { return anchorMin.GetValue(); }
    Vector2 GetAnchorMax() const { return anchorMax.GetValue(); }
    float GetRotation() const { return rotation.GetValue(); }
    RectTransform* GetParent() const { return parent; }
    
    void SetAnchoredPosition(const Vector2& pos) { anchoredPosition.GetValue() = pos; }
    void SetSizeDelta(const Vector2& size) { sizeDelta.GetValue() = size; }
    void SetPivot(const Vector2& newPivot) { pivot.GetValue() = newPivot; }
    void SetRotation(float rot) { rotation.GetValue() = rot; }
    void SetParent(RectTransform* newParent) { parent = newParent; }
    
    void SetAnchors(Vector2 anchorMin, Vector2 anchorMax);
    void SetAnchors(const Vector2& min, const Vector2& max) {
        anchorMin.GetValue() = min;
        anchorMax.GetValue() = max;
    }

    void SetAnchorMin(const Vector2& min) { anchorMin.GetValue() = min; }
    void SetAnchorMax(const Vector2& max) { anchorMax.GetValue() = max; }
    
    void SetSizeAbsolute(float width, float height) {
        sizeDelta.GetValue() = Vector2(width, height);
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

    SERIALIZED_FIELDS(&anchoredPosition, &sizeDelta, &pivot, &anchorMin, &anchorMax, &rotation);
};