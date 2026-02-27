#include "include/core/phisic_engine/new_physiscs/PhysicsManagers/PhysicsWorld.cpp"

void PhysicsWorld::AddCollider(PhysicsBody* body) {
    if (!body) return;
    body->SetPhysicWorld(this);
    bodies.push_back(body);
}

void PhysicsWorld::RemoveBody(PhysicsBody* body){
    auto it = std::find(bodies.begin(), bodies.end(), body);
    
    if (it != bodies.end()) {
        for (auto* other : bodies) {
            if (other != body) {
                other->ClearWorld(body);
            }
        }
        
        bodies.erase(it);
    }
}