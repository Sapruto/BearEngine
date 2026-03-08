#pragma once

#include <memory>
#include <vector>
#include <algorithm>

class PhysicalBody;
class ColliderManager;

class PhysicsWorld{
private:
    std::vector<PhysicalBody*> bodies;

    std::shared_ptr<ColliderManager> colliderManager;

public:
    void SetColliderManager(std::shared_ptr<ColliderManager> colliderManager){ this->colliderManager = colliderManager; }

    void AddBody(PhysicalBody* body);
    
    void RemoveBody(PhysicalBody* body);

    void Start();
    void Update();
};
