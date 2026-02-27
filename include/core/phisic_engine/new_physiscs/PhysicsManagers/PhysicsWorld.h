#pragma once

#include "include/core/phisic_engine/new_physiscs/PhysicalBody/PhysicsBody.h"

class PhysicsWorld{
private:
    std::vector<PhysicsBody*> bodies;

public:
    void AddBody(PhysicsBody* body);
    
    void RemoveBody(PhysicsBody* body);
}
