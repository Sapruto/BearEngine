#include "Gravity.h"

#include "PhysicalBody.h"

Gravity::Gravity(){
    SetLayer(2);
}

void Gravity::Initialize(){
    isValid = true;
    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    float mass = body->GetMass();

    if(!impulseModule || mass == 0){
        isValid = false;
        return;
    }
    
    Vector3 down(0, -1, 0);
    gravitationForce = Force(down, 9.8f * mass);
}

void Gravity::UpdateBody(){
    if(!isValid || !impulseModule) return;

    impulseModule->AddForce(gravitationForce);
}

void Gravity::ReactionOnEvent(BasePhysicsEvent* event){

}

void Gravity::Destroy() {
    impulseModule = nullptr;
    isValid = false;
}