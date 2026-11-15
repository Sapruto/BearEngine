#pragma once

#include "World/Scene/SceneEvent.h"

#include <memory>
#include <vector>
#include <cstdint>

class Scene;
class GameObject;

class SceneEventBus {
private:
    std::vector<std::unique_ptr<SceneEvent>> sceneEvents;

    std::vector<SceneEvent*> onStart;
    std::vector<SceneEvent*> onUpdate;
    std::vector<SceneEvent*> onDestroy;
    std::vector<SceneEvent*> onAddObject;
    std::vector<SceneEvent*> onRemoveObject;

    Scene* owner = nullptr;

    void RebuildIndices() {
        onStart.clear();
        onUpdate.clear();
        onDestroy.clear();
        onAddObject.clear();
        onRemoveObject.clear();

        for (auto& e : sceneEvents) {
            if (!e) continue;
            SceneEvent* raw = e.get();
            const std::uint32_t mask = raw->GetOverriddenMask();

            if (mask & SceneEvent::SceneStart) onStart.push_back(raw);
            if (mask & SceneEvent::SceneUpdate) onUpdate.push_back(raw);
            if (mask & SceneEvent::SceneDestroy) onDestroy.push_back(raw);
            if (mask & SceneEvent::SceneAddObject) onAddObject.push_back(raw);
            if (mask & SceneEvent::SceneRemoveObject) onRemoveObject.push_back(raw);
        }
    }

public:
    SceneEventBus() = default;
    ~SceneEventBus() = default;

    SceneEventBus(const SceneEventBus&) = delete;
    SceneEventBus& operator=(const SceneEventBus&) = delete;

    void SetOwner(Scene* scene);
    Scene* GetOwner() const;

    void OnSceneStart();
    void OnSceneUpdate();
    void OnSceneDestroy();
    void OnAddObject(GameObject* gameObject);
    void OnRemoveObject(GameObject* gameObject);

    SceneEvent* AddSceneEvent(std::unique_ptr<SceneEvent> event);
    void RemoveSceneEvent(SceneEvent* event);

    template<typename T>
    T* GetSceneEvent() {
        for (auto& e : sceneEvents) {
            if (auto* typed = dynamic_cast<T*>(e.get())) return typed;
        }
        return nullptr;
    }
};