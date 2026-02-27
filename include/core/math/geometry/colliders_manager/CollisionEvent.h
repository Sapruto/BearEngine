#pragma once

#include "include/core/math/Vector2.h"
#include <vector>

class Collider;

class CollisionEvent {
public:
    enum class State {
        ENTER,
        STAY,
        EXIT
    };

    CollisionEvent(State state, Collider* self, Collider* other)
        : m_state(state), m_self(self), m_other(other) {}

    State GetState() const { return m_state; }
    Collider* GetSelf() const { return m_self; }
    Collider* GetOther() const { return m_other; }

    bool IsEnter() const { return m_state == State::ENTER; }
    bool IsStay() const { return m_state == State::STAY; }
    bool IsExit() const { return m_state == State::EXIT; }

private:
    State m_state;
    Collider* m_self;
    Collider* m_other;
};