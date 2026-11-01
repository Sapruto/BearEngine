#pragma once

class BaseCollider;

class CollisionEvent  {
public:
    enum class State;

private:
    State state;
    BaseCollider* self;
    BaseCollider* other;

public:
    enum class State {
        ENTER,
        STAY,
        EXIT
    };

    CollisionEvent(State state, BaseCollider* self, BaseCollider* other)
        : state(state), self(self), other(other) {}

    State GetState() const { return state; }
    BaseCollider* GetSelf() const { return self; }
    BaseCollider* GetOther() const { return other; }

    bool IsEnter() const { return state == State::ENTER; }
    bool IsStay() const { return state == State::STAY; }
    bool IsExit() const { return state == State::EXIT; }
};