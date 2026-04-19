#pragma once

class PhysicsWorld;

class BasePhysicManager{
protected:
    PhysicsWorld* world;

    int layer;

public:
    BasePhysicManager() = default;

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;

    void SetLayer(int layer) { this->layer = layer; }
    void SetWorld(PhysicsWorld* world) { this->world = world; }
};