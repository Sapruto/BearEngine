#include "ArchimedForce.h"

#include "LiquidCollisionData.h"
#include "PhysicalBody.h"
#include "Gravity.h"

ArchimedForce::ArchimedForce(){
    SetLayer(7);

    SubcribeEvent(PhysicEventType::LiquidCollision);
}

void ArchimedForce::Initialize(){
    isValid = true;
    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    gravityModule = body->GetFeatureOfType<Gravity>();
    float mass = body->GetMass();

    if(!impulseModule || mass == 0){
        isValid = false;
        return;
    }
}

void ArchimedForce::UpdateBody() {

}
void ArchimedForce::ReactionOnEvent(BasePhysicsEvent* event) {
    if(!isValid) return;

    LiquidCollisionData* collisionEvent = dynamic_cast<LiquidCollisionData*>(event);

    PhysicalBody* liquidBody = collisionEvent->GetLiquidBody();
    if(!liquidBody) return;

    float gravityScale = 9.8f;
    if(gravityModule) gravityScale = gravityModule->GetGravityScale();

    Force archimedForce = Force(collisionEvent->GetDirection() , liquidBody->GetDensity() * collisionEvent->GetVolumeInLiquid() * gravityScale);

    impulseModule->AddForce(archimedForce);
}

void ArchimedForce::Destroy() {

}