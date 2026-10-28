#pragma once

#include "Component.h"
#include "Keys.h"
#include "Vector2.h"
#include "Canvas.h"

#include <functional>
#include <map>

class Image;

class Button : public Component{
public:
    using EventHandler = std::function<void()>;

    enum class ClickState {
        Released = 0,
        Pressed,
        JustPressed,
        JustReleased  
    };

private:
    Canvas* canvass{nullptr};

    Image* uiElement;

    MouseButton mouseButton;
    ClickState state;

    std::map<ClickState, std::vector<EventHandler>> subscriptions;

    bool CheckMousePos(const Vector2f& globalMousePos, Canvas* canvas);

public:
    void Start() override;
    void Update() override;
    void Destroy() override;

    void SetUIElement(Image* uiElement) { this->uiElement = uiElement; }
    void SetMouseButton(MouseButton mouseButton) { this->mouseButton = mouseButton; }

    Image* GetUIElement() { return uiElement; }
    MouseButton GetMouseButton() { return mouseButton; }

    ClickState GetState() { return state; }

    void Unsubscribe(ClickState eventState, EventHandler callback);
    void ClearAllSubscriptions(){
        subscriptions.clear();
    }

    void Subscribe(ClickState eventState, EventHandler callback) {
        subscriptions[eventState].push_back(callback);
    }

    void SetCanvas(Canvas* cv) { canvass = cv; }
};