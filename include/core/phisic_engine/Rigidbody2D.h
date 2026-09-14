#pragma once

#include <vector>
#include <memory>
#include "Transform2D.h"
#include "Vector2.h"
#include "Component.h"
#include "Force.h"

class RigidBody2D : public Component {
private:
    Transform2D* transform;
    std::vector<Force> forces;
    Vector2f velocity;
    Vector2f acceleration;
    float mass;
    float drag;
    
public:
    RigidBody2D(float mass = 1.0f, float drag = 0.1f);
    ~RigidBody2D();
    
    void Start() override;
    
    void Update() override;
    
    void AddForce(const Vector2f& force);
    
    void AddForce(const Vector2f& direction, float magnitude);
    
    void AddImpulse(const Vector2f& impulse);
    
    void SetVelocity(const Vector2f& vel);
    Vector2f GetVelocity() const;
    
    void SetMass(float newMass);
    float GetMass() const;

    void SetDrag(float newDrag);
    float GetDrag() const;
    
    Transform2D* GetTransform() const;
};