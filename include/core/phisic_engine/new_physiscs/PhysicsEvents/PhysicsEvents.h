#pragma once

#include <unordered_map>

enum class PhysicEventType{
    ForceInteraction,
    CollisionEvent,   
    CollisionEnter,  
    CollisionStay,      
    CollisionExit,     
    TriggerEnter,       
    TriggerStay,       
    TriggerExit    
};

class AbstractPhysicsData {
public:
    virtual ~AbstractPhysicsData() = default;
};

class BasePhysicsEvent {
private:
    AbstractPhysicsData* data;
    PhysicEventType type;

public:
    BasePhysicsEvent(AbstractPhysicsData* data, PhysicEventType type) 
        : data(data), type(type) { }
    
    virtual ~BasePhysicsEvent() = default;

    AbstractPhysicsData* GetData() { return data; }
    PhysicEventType GetType() const { return type; }
};