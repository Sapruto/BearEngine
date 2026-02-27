#pragma once

#include <vector>
#include <memory>
#include "include/core/math/Transform2D.h"
#include "include/core/math/Vector2.h"
#include "include/core/system_engine/component_system/Component.h"

struct Force {
    Vector2 direction;
    float magnitude;
    
    Force() : direction(0, 0), magnitude(0) {}
    Force(const Vector2& dir, float mag) : direction(dir.normalized()), magnitude(mag) {}
    
    Vector2 GetForceVector() const {
        return direction * magnitude;
    }
};

class RigidBody2D : public Component {
private:
    Transform2D* transform;
    std::vector<Force> forces;
    Vector2 velocity;
    Vector2 acceleration;
    float mass;
    float drag;
    
public:
    RigidBody2D(float mass = 1.0f, float drag = 0.1f);
    ~RigidBody2D();
    
    void Start() override;
    
    void Update() override;
    
    void AddForce(const Vector2& force);
    
    void AddForce(const Vector2& direction, float magnitude);
    
    void AddImpulse(const Vector2& impulse);
    
    void SetVelocity(const Vector2& vel);
    Vector2 GetVelocity() const;
    
    void SetMass(float newMass);
    float GetMass() const;

    void SetDrag(float newDrag);
    float GetDrag() const;
    
    Transform2D* GetTransform() const;
};