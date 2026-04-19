#include "Newtonian.h"
#include "Transform3D.h"
#include "Time.h"

#include "PhysicalBody.h"

ImpulseModule::ImpulseModule() : velocity(Vector3::Zero){
    SubcribeEvent(PhysicEventType::ForceInteraction);
    SetLayer(0);
}

ImpulseModule::~ImpulseModule() = default;

void ImpulseModule::ApplyArcadyChange(){
    velocity *= damping;

    float speed = velocity.magnitude();
    if (speed > maxSpeed) {
        velocity = velocity * (maxSpeed / speed);
    }

    Transform3D* transform = body->GetTransform();
    if (!transform) return;
    
    Vector3 pos = transform->GetLocalPosition();
    
    if (fabs(pos.x) > worldLimit) {
        pos.x = (pos.x > 0) ? worldLimit : -worldLimit;
        transform->SetLocalPosition(pos);
        velocity.x *= -0.5f;
    }
}

void ImpulseModule::AddForce(Vector3 direction, float magnitude){
    AddForce(Force(direction, magnitude));
}

void ImpulseModule::AddForce(const Force& new_force){
    if (new_force.magnitude == 0.0f) return;
    
    Vector3 current = main_force.GetForceVector();
    Vector3 added = new_force.GetForceVector();
    Vector3 total = current + added;
    
    float mag = total.magnitude();
    if (mag > 0) {
        main_force.direction = total / mag;
        main_force.magnitude = mag;
    }
}

void ImpulseModule::ReForce(const Force& new_force){
    main_force = new_force;
    velocity = Vector3::Zero;
}

void ImpulseModule::UpdateBody(){
    float deltaTime = Time::DeltaTime();
    float mass = body->GetMass();
    Transform3D* transform = body->GetTransform();

    if (mass <= 0.0f || !transform) {
        return; 
    }

    Vector3 force_vector = main_force.GetForceVector();
    Vector3 acceleration = force_vector / mass; 

    velocity += acceleration * deltaTime; 
        
    Vector3 localPos = transform->GetLocalPosition();
    
    localPos += velocity * deltaTime;
    
    transform->SetLocalPosition(localPos);
    
    if (mode == ImpulseModuleMode::ARCADY) {
        ApplyArcadyChange();
    }
    
    main_force = Force(Vector3::Zero, 0.0f);
}

void ImpulseModule::ReactionOnEvent(BasePhysicsEvent* event){
    AbstractPhysicsData* events_data = event->GetData();

    auto* force_data = dynamic_cast<ImpulsePhysicsData*>(events_data);
    if (!force_data) { 
        return; 
    }

    const auto& forces = force_data->GetForces();
    for (const auto& force : forces) { 
        AddForce(force);
    }
}

void ImpulseModule::Destroy(){

}