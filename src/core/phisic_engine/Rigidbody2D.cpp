#include <vector>
#include "include/core/phisic_engine/Rigidbody2D.h"
#include "include/core/GameObject.h"
#include "include/core/Component.h"

#include "include/core/math/Vector2.h"
#include "include/core/math/Transform2D.h"

RigidBody2D::RigidBody2D(float mass, float drag) 
        : mass(mass), drag(drag), transform(nullptr), velocity(0, 0), acceleration(0, 0) {}

RigidBody2D::~RigidBody2D() {}

void RigidBody2D::Start(){
    Component::Start();
    transform = gameObject->GetComponentOfType<Transform2D>();
}

void RigidBody2D::Update(float deltaTime){
    Component::Update(deltaTime);
    if (!transform || mass <= 0 || deltaTime <= 0) return;
    
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    
    Vector2 totalForce(0, 0);
    for (const auto& force : forces) {
        totalForce += force.GetForceVector();
    }
    
    acceleration = totalForce / mass;
    
    velocity += acceleration * deltaTime;
    
    float dragFactor = 1.0f - drag * deltaTime;
    if (dragFactor < 0) dragFactor = 0;
    if (dragFactor > 1) dragFactor = 1;
    velocity *= dragFactor;
    
    transform->position += velocity * deltaTime;
    
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