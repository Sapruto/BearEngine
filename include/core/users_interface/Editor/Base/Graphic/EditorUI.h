#pragma once

#include "GameObject.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include "Canvas.h"
#include "BaseDrawData.h"

class EditorUI {
protected:
    Canvas* canvas;

    std::unordered_map<GameObject*, std::unique_ptr<BaseDrawData>> objectsToDrawData;

public:
    void SetCanvas(Canvas* canvas) { this->canvas = canvas; }

    template<typename T>
    void AddObject(GameObject* object, std::unique_ptr<T> data) {
        static_assert(std::is_base_of_v<BaseDrawData, T>, "T must derive from BaseDrawData");
        objectsToDrawData[object] = std::move(data);
    }
    
    template<typename T>
    T* GetDrawData(GameObject* object) {
        auto it = objectsToDrawData.find(object);
        if (it != objectsToDrawData.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    virtual void DrawObject(GameObject* object, BaseDrawData& data) = 0;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
};