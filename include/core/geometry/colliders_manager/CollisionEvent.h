#pragma once

#include "PhysicsEvents.h"

class BaseCollider;

class CollisionEvent : public AbstractPhysicsData  {
public:
    enum class State {
        ENTER,
        STAY,
        EXIT
    };

    CollisionEvent(State state, BaseCollider* self, BaseCollider* other)
        : m_state(state), m_self(self), m_other(other) {}

    State GetState() const { return m_state; }
    BaseCollider* GetSelf() const { return m_self; }
    BaseCollider* GetOther() const { return m_other; }

    bool IsEnter() const { return m_state == State::ENTER; }
    bool IsStay() const { return m_state == State::STAY; }
    bool IsExit() const { return m_state == State::EXIT; }

private:
    State m_state;
    BaseCollider* m_self;
    BaseCollider* m_other;
};