#include <vector>
#include "include/core/phisic_engine/Rigidbody2D.h"
#include "include/core/GameObject.h"
#include "include/core/system_engine/component_system/Component.h"

#include "include/core/math/Vector2.h"
#include "include/core/math/Transform2D.h"
#include "include/core/system_engine/time_system/Time.h"

RigidBody2D::RigidBody2D(float mass, float drag) 
        : mass(mass), drag(drag), transform(nullptr), velocity(0, 0), acceleration(0, 0) {}

RigidBody2D::~RigidBody2D() {}

void RigidBody2D::Start(){
    transform = gameObject->GetComponentOfType<Transform2D>();
}

void RigidBody2D::Update(){
    float dt = Time::DeltaTime();
    if (!transform || mass <= 0 || dt <= 0) return;
    if (dt > 0.1f) dt = 0.1f;
    velocity += acceleration * dt;
    
    Vector2 totalForce(0, 0);
    for (const auto& force : forces) {
        totalForce += force.GetForceVector();
    }
    
    acceleration = totalForce / mass;
    
    velocity += acceleration * dt;
    
    float dragFactor = 1.0f - drag * dt;
    if (dragFactor < 0) dragFactor = 0;
    if (dragFactor > 1) dragFactor = 1;
    velocity *= dragFactor;
    
    transform->position += velocity * dt;
    
    forces.clear();
}

void RigidBody2D::AddForce(const Vector2& force) {
    forces.push_back(Force(force.normalized(), force.magnitude()));
}
void RigidBody2D::AddForce(const Vector2& direction, float magnitude) {
    forces.push_back(Force(direction, magnitude));
}

void RigidBody2D::AddImpulse(const Vector2& impulse) {
    velocity += impulse / mass;
}

void RigidBody2D::SetVelocity(const Vector2& vel) { velocity = vel; }
Vector2 RigidBody2D::GetVelocity() const { return velocity; }

void RigidBody2D::SetMass(float newMass) { mass = newMass > 0 ? newMass : 1.0f; }
float RigidBody2D::GetMass() const { return mass; }

void RigidBody2D::SetDrag(float newDrag) { drag = newDrag; }
float RigidBody2D::GetDrag() const { return drag; }

Transform2D* RigidBody2D::GetTransform() const { return transform; }