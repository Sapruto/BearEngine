#include "World/Scene/SceneEventBus.h"

#include "World/Scene/Scene.h"
#include "World/Scene/SceneEvent.h"
#include "World/GameObject.h"

#include <algorithm>

void SceneEventBus::SetOwner(Scene* scene) { owner = scene; }
Scene* SceneEventBus::GetOwner() const { return owner; }

void SceneEventBus::OnSceneStart() {
    for (auto* e : onStart) {
        e->OnSceneStart();
    }
}

void SceneEventBus::OnSceneUpdate() {
    for (auto* e : onUpdate) {
        if (e->IsEnabled()) e->OnSceneUpdate();
    }
}

void SceneEventBus::OnSceneDestroy() {
    for (auto* e : onDestroy) {
        e->OnSceneDestroy();
    }
    sceneEvents.clear();
    RebuildIndices();
}

void SceneEventBus::OnAddObject(GameObject* gameObject) {
    if (!gameObject) return;
    for (auto* e : onAddObject) {
        if (e->IsEnabled()) e->OnAddObject(gameObject);
    }
}

void SceneEventBus::OnRemoveObject(GameObject* gameObject) {
    if (!gameObject) return;
    for (auto* e : onRemoveObject) {
        if (e->IsEnabled()) e->OnRemoveObject(gameObject);
    }
}

SceneEvent* SceneEventBus::AddSceneEvent(std::unique_ptr<SceneEvent> event) {
    if (!event) return nullptr;
    if (owner) event->SetScene(owner);

    sceneEvents.push_back(std::move(event));

    RebuildIndices();

    return sceneEvents[sceneEvents.size() - 1].get();
}

void SceneEventBus::RemoveSceneEvent(SceneEvent* event) {
    if (!event) return;

    auto it = std::find_if(sceneEvents.begin(), sceneEvents.end(),
        [event](const std::unique_ptr<SceneEvent>& e) {
            return e.get() == event;
        });

    if (it == sceneEvents.end()) return;

    sceneEvents.erase(it);
    RebuildIndices();
}