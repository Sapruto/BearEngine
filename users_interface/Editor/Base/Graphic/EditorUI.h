#pragma once

#include "GameObject.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include "Canvas.h"
#include "UIElement.h"

template<typename TDataVariant>
class EditorUI {
protected:
    Canvas* canvas;

    std::unordered_map<GameObject*, UIElement*> objectsUIToUIElements;

public:
    virtual ~EditorUI() = default;

    void SetCanvas(Canvas* canvas) { this->canvas = canvas; }

    virtual void AddObject(GameObject* object, const TDataVariant& data) = 0;
    virtual void UpdateUIObject(GameObject* object, const TDataVariant& data) = 0;
    virtual void DeleteObjectFromUI(GameObject* object) = 0;

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
};