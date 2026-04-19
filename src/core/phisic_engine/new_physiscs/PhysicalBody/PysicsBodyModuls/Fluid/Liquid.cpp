#include "Liquid.h"

#include "CollisionEvent.h"
#include "BaseCollider.h"
#include "PhysicsWorld.h"
#include "PhysicalBody.h"

#include "LiquidCollisionData.h"

Liquid::Liquid(const std::vector<std::pair<Vertex3D, Vertex3D>>& limiterPoints, const std::vector<Vertex3D>& spawnPoints)
 : limiterPoints(limiterPoints), spawnPoints(spawnPoints){
    SetLayer(6);

    SubcribeEvent(PhysicEventType::CollisionEvent);
}

void Liquid::Initialize() {
    isValid = false;

    transform = body->GetTransform();
    BaseCollider* collider = body->GetBaseCollider();
    if(!transform || !collider) return;

    collider->SetIsTrigger(true);

    std::vector<Vertex3D> mainPoints;
    for(const auto& spawnPoint : spawnPoints) {
        Vertex3D mainPoint = spawnPoint; 
        for(const auto& limiterPoint : limiterPoints) {
            if(mainPoint.localPoint.x < limiterPoint.first.localPoint.x)  
                mainPoint.localPoint.x = limiterPoint.first.localPoint.x;
            if(mainPoint.localPoint.y < limiterPoint.first.localPoint.y)  
                mainPoint.localPoint.y = limiterPoint.first.localPoint.y;
            if(mainPoint.localPoint.z < limiterPoint.first.localPoint.z)  
                mainPoint.localPoint.z = limiterPoint.first.localPoint.z;

            if(mainPoint.localPoint.x > limiterPoint.second.localPoint.x)  
                mainPoint.localPoint.x = limiterPoint.second.localPoint.x;
            if(mainPoint.localPoint.y > limiterPoint.second.localPoint.y)  
                mainPoint.localPoint.y = limiterPoint.second.localPoint.y;
            if(mainPoint.localPoint.z > limiterPoint.second.localPoint.z)  
                mainPoint.localPoint.z = limiterPoint.second.localPoint.z;
        }
        mainPoints.push_back(mainPoint);
    }
    
    this->mainPoints = mainPoints;
    isValid = true;
}

void Liquid::UpdateBody(){
    if(!isValid) return;


}

void Liquid::ReactionOnEvent(BasePhysicsEvent* event){
    if(!isValid) return;

    CollisionEvent* collisitonEvent = dynamic_cast<CollisionEvent*>(event);
    if(!collisitonEvent) return;

    BaseCollider* selfColliderFromEvent = collisitonEvent->GetSelf();
    BaseCollider* otherColliderFromEvent = collisitonEvent->GetOther();

    if(!selfColliderFromEvent || !otherColliderFromEvent) return;

    BaseCollider* collider = body->GetBaseCollider();

    if(selfColliderFromEvent != collider && otherColliderFromEvent != collider) return;

    BaseCollider* otherColliderFromMe = nullptr;
    if(otherColliderFromEvent != collider){
        otherColliderFromMe = otherColliderFromEvent;
    }
    else if(selfColliderFromEvent != collider){
        otherColliderFromMe = selfColliderFromEvent;
    }

    PhysicalBody* otherBody = otherColliderFromMe->gameObject->GetComponentOfType<PhysicalBody>();
    if(!otherBody) return;

    LiquidCollisionData collisionData(body);

    PhysicsWorld* world = const_cast<PhysicsWorld*>(body->GetWorld());
    if (world) {
        world->DispatchEventToBody(PhysicEventType::LiquidCollision, collisionData, otherBody);
    }
}

void Liquid::Destroy(){
    if(!isValid) return;
    
}

void Liquid::RefactorModel(){
    if(!isValid) return;

}