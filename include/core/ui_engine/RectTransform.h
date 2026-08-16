#pragma once

#include "Component.h"
#include "Vector2.h"
#include "UIRect.h"
#include "SerializeField.h"

class RectTransform : public Component {
private:
    FIELD(Vector2f, anchoredPosition);
    FIELD(Vector2f, sizeDelta);
    FIELD(Vector2f, pivot);
    FIELD(Vector2f, anchorMin);   
    FIELD(Vector2f, anchorMax);
    FIELD(float, rotation);
    
    RectTransform* parent;  
    
public:
    RectTransform();
    
    Vector2f GetAnchoredPosition() const { return anchoredPosition.GetValue(); }
    Vector2f GetSizeDelta() const { return sizeDelta.GetValue(); }
    Vector2f GetPivot() const { return pivot.GetValue(); }
    Vector2f GetAnchorMin() const { return anchorMin.GetValue(); }
    Vector2f GetAnchorMax() const { return anchorMax.GetValue(); }
    float GetRotation() const { return rotation.GetValue(); }
    RectTransform* GetParent() const { return parent; }
    
    void SetAnchoredPosition(const Vector2f& pos) { anchoredPosition.GetValue() = pos; }
    void SetSizeDelta(const Vector2f& size) { sizeDelta.GetValue() = size; }
    void SetPivot(const Vector2f& newPivot) { pivot.GetValue() = newPivot; }
    void SetRotation(float rot) { rotation.GetValue() = rot; }
    void SetParent(RectTransform* newParent) { parent = newParent; }
    
    void SetAnchors(Vector2f anchorMin, Vector2f anchorMax);
    void SetAnchors(const Vector2f& min, const Vector2f& max) {
        anchorMin.GetValue() = min;
        anchorMax.GetValue() = max;
    }

    void SetAnchorMin(const Vector2f& min) { anchorMin.GetValue() = min; }
    void SetAnchorMax(const Vector2f& max) { anchorMax.GetValue() = max; }
    
    void SetSizeAbsolute(float width, float height) {
        sizeDelta.GetValue() = Vector2f(width, height);
    }
    
    UIRect GetScreenRect(float screenWidth, float screenHeight) const;
    
    Vector2f GetScreenPosition(float screenWidth, float screenHeight) const {
        UIRect rect = GetScreenRect(screenWidth, screenHeight);
        return Vector2f(rect.x, rect.y);
    }
    
    Vector2f GetScreenSize(float screenWidth, float screenHeight) const {
        UIRect rect = GetScreenRect(screenWidth, screenHeight);
        return Vector2f(rect.width, rect.height);
    }

    SERIALIZED_FIELDS(&anchoredPosition, &sizeDelta, &pivot, &anchorMin, &anchorMax, &rotation);
};