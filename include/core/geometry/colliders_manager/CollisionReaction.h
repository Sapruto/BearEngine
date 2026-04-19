#pragma once

#include "CollisionEvent.h"

#include <functional>
#include <map>

class BaseCollider;

class CollisionReaction {
public:
    using EventHandler = std::function<void(BaseCollider* self, BaseCollider* other)>;

private:
    std::map<CollisionEvent::State, std::vector<EventHandler>> subscriptions;
    
    void AddState(CollisionEvent::State newState) {
        subscriptions[newState];
    }

public:
    CollisionReaction() {
        for (auto state : {CollisionEvent::State::ENTER, 
                           CollisionEvent::State::STAY, 
                           CollisionEvent::State::EXIT}) {
            subscriptions[state] = {};
        }
    }

    void Subscribe(EventHandler handler, CollisionEvent::State state) {
        subscriptions[state].push_back(handler);
    }
    
    void Trigger(CollisionEvent::State state, BaseCollider* self, BaseCollider* other) {
        auto it = subscriptions.find(state);
        if (it != subscriptions.end()) {
            for (auto& handler : it->second) {
                handler(self, other);
            }
        }
    }

    void UnsubscribeAll(CollisionEvent::State state) {
        if (auto it = subscriptions.find(state); it != subscriptions.end()) {
            it->second.clear();
        }
    }
};