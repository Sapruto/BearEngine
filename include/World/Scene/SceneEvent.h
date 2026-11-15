#pragma once

#include <cstdint>
#include <type_traits>

class Scene;
class GameObject;

class SceneEvent {
public:
    enum Flags : std::uint32_t {
        None = 0,
        SceneStart = 1u << 0,
        SceneUpdate = 1u << 1,
        SceneDestroy = 1u << 2,
        SceneAddObject = 1u << 3,
        SceneRemoveObject = 1u << 4,
    };

protected:
    Scene* scene = nullptr;
    bool enabled = true;
    std::uint32_t overridden = None;

public:
    virtual ~SceneEvent() = default;

    void SetScene(Scene* s) { scene = s; }
    Scene* GetScene() const { return scene; }

    virtual void OnSceneStart()   {}
    virtual void OnSceneUpdate()  {}
    virtual void OnSceneDestroy() {}

    virtual void OnAddObject([[maybe_unused]] GameObject* gameObject)  {}
    virtual void OnRemoveObject([[maybe_unused]] GameObject* gameObject)  {}

    bool IsEnabled() const { return enabled; }
    void SetEnabled(bool e) { enabled = e; }

    bool Has(Flags f) const { return (overridden & f) != 0; }
    std::uint32_t GetOverriddenMask() const { return overridden; }
};

template<typename Derived>
class SceneEventT : public SceneEvent {
protected:
    SceneEventT() {
        if constexpr (!std::is_same_v<
                decltype(&Derived::OnSceneStart),
                decltype(&SceneEvent::OnSceneStart)>) {
            overridden |= SceneStart;
        }
        if constexpr (!std::is_same_v<
                decltype(&Derived::OnSceneUpdate),
                decltype(&SceneEvent::OnSceneUpdate)>) {
            overridden |= SceneUpdate;
        }
        if constexpr (!std::is_same_v<
                decltype(&Derived::OnSceneDestroy),
                decltype(&SceneEvent::OnSceneDestroy)>) {
            overridden |= SceneDestroy;
        }
        if constexpr (!std::is_same_v<
                decltype(&Derived::OnAddObject),
                decltype(&SceneEvent::OnAddObject)>) {
            overridden |= SceneAddObject;
        }
        if constexpr (!std::is_same_v<
                decltype(&Derived::OnRemoveObject),
                decltype(&SceneEvent::OnRemoveObject)>) {
            overridden |= SceneRemoveObject;
        }
    }
};