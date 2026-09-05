#pragma once

#include "Button.h"
#include "Component.h"

#include <functional>
#include <map>

class Toggle : public Component {
public:
    using EventHandler = std::function<void()>;

private:
    Button* button;

    bool state;

    std::map<bool, std::vector<EventHandler>> subscriptions;

    void ProccesEvents();

public:
    Toggle() = default;
    Toggle(Button* btn);

    void Start() override;

    void ChangeState();

    bool GetState();
};