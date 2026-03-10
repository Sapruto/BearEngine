#include "Friction.h"

#include "CollisionEvent.h"
#include "Polygon3D.h"

Friction::Friction(float mu) : mu(mu), defaultOtherMu(0.5f) { 
    SubcribeEvent(PhysicEventType::CollisionEvent);
}

 void Friction::Initialize(){
    isValid = true;

    impulseModule = body->GetFeatureOfType<ImpulseModule>();

    if(!impulseModule) isValid = false;
 }

float Friction::GetOtherMu(GameObject* otherObject){
    if(!otherObject) return defaultOtherMu;
    
    PhysicalBody* otherBody = otherObject->GetComponentOfType<PhysicalBody>();
    if(!otherBody) return defaultOtherMu;
    
    Friction* friction = otherBody->GetFeatureOfType<Friction>();
    if(!friction) return defaultOtherMu;
    
    return friction->GetMu();
}

void Friction::CalculateNormalForce(){
    if(!isValid) return;

    Force mainForce = impulseModule->GetForce();
    Vector3 velocity = impulseModule->GetVelocity();

    contacts.clear();
    N = 0.0f;
        
    for(const auto& normal : normals){
        /* GameObject* otherObject = normal.GetCollisionsGameObject();

        float otherMu = GetOtherMu(otherObject); */
        float otherMu = 0.0f;
        float contactMu = (otherMu + mu) / 2.0f;

        float normalForceForThisPlane = 0;
        float projection = mainForce.direction.dot(normal);
        if (projection < 0) {
            normalForceForThisPlane = -projection * mainForce.magnitude;
            N += normalForceForThisPlane;
        }
        
        Vector3 velocityProjection = normal * (velocity.dot(normal));
        Vector3 tangentialVelocity = velocity - velocityProjection;
        
        if (tangentialVelocity.magnitude() > 0.0001f) {
            Vector3 frictionDir = -tangentialVelocity.normalized();
            float frictionMagnitude = contactMu * normalForceForThisPlane;
            
            ContactData contact;
            contact.frictionForce = frictionDir * frictionMagnitude;
            contact.contactMu = contactMu;
            contact.normalForce = normalForceForThisPlane;
            
            contacts.push_back(contact);
        }
    }
}

void Friction::ChangeBody(){
    if(!isValid) return;

    Force mainForce = impulseModule->GetForce();
    Vector3 velocity = impulseModule->GetVelocity();

    if (N <= 0.0001f) return; 
    
    if (velocity.magnitude() >= 0.0001f) {
        Vector3 totalFrictionForce(0, 0, 0);
        
        for(const auto& contact : contacts){
            totalFrictionForce += contact.frictionForce;
        }
        
        float weightedMu = 0.0f;
        for(const auto& contact : contacts){
            weightedMu += contact.contactMu * contact.normalForce;
        }
        float maxFriction = weightedMu;
        
        if (totalFrictionForce.magnitude() > maxFriction) {
            totalFrictionForce = totalFrictionForce.normalized() * maxFriction;
        }
        
        if (totalFrictionForce.magnitude() > 0.0001f) {
            impulseModule->AddForce(Force(
                totalFrictionForce.normalized(), 
                totalFrictionForce.magnitude()
            ));
        }
    } 
    else {
        float weightedMu = 0.0f;
        for(const auto& contact : contacts){
            weightedMu += contact.contactMu * contact.normalForce;
        }
        float maxFriction = weightedMu;
        
        float totalForceMagnitude = mainForce.magnitude;
        if (totalForceMagnitude > maxFriction) {
            Vector3 frictionDir = -mainForce.direction;
            impulseModule->AddForce(Force(frictionDir, maxFriction));
        }
    }
}

void Friction::ReactionOnEvent(BasePhysicsEvent* event) {
    if(!isValid) return;
    if (event->GetType() != PhysicEventType::CollisionEvent) return;
    
    auto* collisionEvent = dynamic_cast<CollisionEvent*>(event->GetData());
    if (!collisionEvent) return;
    
    Polygon3D* myCollider = dynamic_cast<Polygon3D*>(body->GetBaseCollider());
    Polygon3D* otherCollider = dynamic_cast<Polygon3D*>(collisionEvent->GetOther());
    
    if (!myCollider || !otherCollider) return;
    
    if (collisionEvent->GetState() != CollisionEvent::State::STAY) return;
    
    auto segments = myCollider->GetIntersectionSegments(*otherCollider);
    
    normals.clear();
    CalculateNormal(segments); 
    CalculateNormalForce();
}

void Friction::Destroy(){

}