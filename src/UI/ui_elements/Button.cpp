#include "Button.h"

#include "Image.h"
#include "InputSystem.h"

Vector2f MousePosToCanvasPos(const Vector2f& mousePos, Canvas* canvas){
    return mousePos; // пока так, а потом когда у Canvas может быть много состояний надо сделать типа преобразование нормальное
}

bool Button::CheckMousePos(const Vector2f& globalMousePos, Canvas* canvas){
    Vector2f realPos = MousePosToCanvasPos(globalMousePos, canvas);

    UIElement* overlayElement = canvas->GetOverlayElement(realPos);
    if(overlayElement != uiElement) return false;
    
    return true;
}

void Button::Start(){
    if(!uiElement){
        uiElement = gameObject->GetComponentOfType<Image>();
        if(!uiElement) gameObject->AddComponent<Image>();
        uiElement = gameObject->GetComponentOfType<Image>();
    }

    if(uiElement && uiElement->GetCanvas()) {
        canvass = uiElement->GetCanvas();
    }
}

void Button::Update(){
    if(!uiElement) return;

    Vector2f globalMousePos = Vector2f(InputSystem::GetInstance().GetMouseX(), InputSystem::GetInstance().GetMouseY());
    bool isClicked = InputSystem::GetInstance().GetMouseButtonDown(static_cast<int>(mouseButton));
    bool isPressed = InputSystem::GetInstance().GetMouseButton(static_cast<int>(mouseButton));

    Canvas* canvas = canvass;
    if(!canvas && uiElement) {
        canvas = uiElement->GetCanvas();
        if(canvas) canvass = canvas;
    }
    
    if(!canvas) {
        std::cout << "Button has no canvas! uiElement=" << uiElement << std::endl;
        return;
    }
    
    if(!CheckMousePos(globalMousePos, canvas)){
        if(state != ClickState::Released) {
            state = ClickState::Released;
        }
        return;
    }

    ClickState previousState = state;
    
    if(isPressed){
        if(state == ClickState::Released || state == ClickState::JustReleased){
            state = ClickState::JustPressed;
        } 
        else if(state == ClickState::JustPressed){
            state = ClickState::Pressed;
        }
    } 
    else{
        if(state == ClickState::Pressed || state == ClickState::JustPressed){
            state = ClickState::JustReleased;
        } 
        else if(state == ClickState::JustReleased){
            state = ClickState::Released;
        }
    }
    
    if(subscriptions.count(state)) {
        for(auto& callback : subscriptions[state]) {
            callback();
        }
    }
}

void Button::Destroy(){
    ClearAllSubscriptions();
    
    uiElement = nullptr;
    
    Component::Destroy();
}

void Button::Unsubscribe(ClickState eventState, EventHandler callback){
    auto it = subscriptions.find(eventState);
    if(it != subscriptions.end()){
        auto& callbacks = it->second;
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                [&callback](const EventHandler& stored){
                    return stored.target_type() == callback.target_type() &&
                           stored.target<void()>() == callback.target<void()>();
                }),
            callbacks.end()
        );
        
        if(callbacks.empty()){
            subscriptions.erase(it);
        }
    }
}