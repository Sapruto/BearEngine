#include "Toggle.h"

Toggle::Toggle(Button* btn) : button(btn) {}

void Toggle::ProccesEvents() {
    if(subscriptions.count(state)) {
        for(auto& callback : subscriptions[state]) {
            callback();
        }
    }
}

void Toggle::Start() {
    if (!button) {
        button = gameObject->GetComponentOfType<Button>();
        if(!button) button = gameObject->AddComponent<Button>();
        if(!button) return;
    }

    button->Subscribe(Button::ClickState::JustPressed, [this]() {
        this->ChangeState();
    });
}

void Toggle::ChangeState() {
    state = !state;
    ProccesEvents();
}

bool Toggle::GetState() {
    return state;
}