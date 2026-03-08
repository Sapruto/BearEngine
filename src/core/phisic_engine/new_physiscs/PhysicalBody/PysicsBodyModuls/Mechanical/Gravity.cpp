#include "Gravity.h"

#include "PhysicalBody.h"

void Gravity::Initialize(){
    isValid = true;

    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    float mass = body->GetMass();

    if(!impulseModule){
        isValid = false;
        return;
    }
    if(mass == 0){
        isValid = false;
        return;
    }
    
    gravitationForce = Force(Vector3::Down, gravitationScale * mass);
}

void Gravity::ChangeBody(){
    if(!isValid || !impulseModule) return;

    impulseModule->AddForce(gravitationForce);
}

void Gravity::ReactionOnEvent(BasePhysicsEvent* event){

}

void Gravity::Destroy() {
    impulseModule = nullptr;
    isValid = false;
}