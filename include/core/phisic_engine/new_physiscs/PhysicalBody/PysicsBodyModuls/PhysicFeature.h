#pragma once

class PhysicalBody;

class PhysicFeature{
protected:
    PhysicalBody* body;
public:
    Feature() = default; 
    virtual ~Feature() = default;

    void Initialize(PhysicalBody* body){
        this->body = body;
    }
    
    virtual void ChangeBody() = 0;
    virtual void ReactionOnEvent() = 0;
    virtual void GetStateFeature() = 0;
};