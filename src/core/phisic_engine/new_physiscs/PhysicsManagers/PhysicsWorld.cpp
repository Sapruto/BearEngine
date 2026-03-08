#include "PhysicsWorld.h"

#include "PhysicalBody.h"

void PhysicsWorld::AddBody(PhysicalBody* body) {
    if (!body) return;
    body->SetPhysicWorld(this);
    bodies.push_back(body);
}

void PhysicsWorld::RemoveBody(PhysicalBody* body){
    auto it = std::find(bodies.begin(), bodies.end(), body);
    
    if (it != bodies.end()) {
        for (auto* other : bodies) {
            if (other != body) {
                other->ClearWorld();
            }
        }
        
        bodies.erase(it);
    }
}

void PhysicsWorld::Start(){
    for(auto body : bodies){
        body->SetPhysicWorld(this);
        body->Initialize();
    }
}

void PhysicsWorld::Update(){
    for(auto body : bodies){
        body->Update();
    }
}