#include "Newtonian.h"
#include "Transform3D.h"
#include "Time.h"

#include "PhysicalBody.h"

ImpulseModule::ImpulseModule(){
    SubcribeEvent(PhysicEventType::ForceInteraction);
}

ImpulseModule::~ImpulseModule() = default;

void ImpulseModule::AddForce(Vector3 direction, float magnitude){
    if (direction == Vector3::Zero) return;
    if (magnitude == 0.0f) return;

    Force new_force(direction, magnitude);
    main_force += new_force;
}

void ImpulseModule::AddForce(const Force& new_force){
    if (new_force.magnitude == 0.0f) return;
    
    main_force += new_force; 
}

void ImpulseModule::ChangeBody(){
    float deltaTime = Time::DeltaTime();
    float mass = body->GetMass();
    Transform3D* transform = body->GetTransform();

    if (mass <= 0.0f) return; 

    Vector3 force_vector = main_force.GetForceVector();
    Vector3 acceleration = force_vector / mass; 

    velocity += acceleration * deltaTime; 
    transform->position += velocity * deltaTime;

    main_force = Force(); 
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