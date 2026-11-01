#include "Domains/Physics/Core/PhysicDomain.h"
#include "Systems/Time/TimeSystem.h"

using namespace Physic;

PhysicDomain::PhysicDomain() {
    auto require = GetRequire();
    require->AddSystemByType<TimeSystem>();
}

void PhysicDomain::Sort() {
    if (!needSort) return;

    std::stable_sort(simulators.begin(), simulators.end(),
        [](const std::unique_ptr<IPhysicSimulator>& a,
            const std::unique_ptr<IPhysicSimulator>& b) {
            return a->GetLayer() < b->GetLayer();
        });

    needSort = false;
}

void PhysicDomain::StartDomain() {
    Sort();
    
    auto& timeSystem = *GetRequire()->GetSystemByType<TimeSystem>();
    for (auto& simulator : simulators) {
        simulator->SetTimeSystem(timeSystem);
        simulator->SetEventBus(eventBus);
        simulator->StartSimulation();
    }
    isStarted = true;
}

void PhysicDomain::BeginFrameDomain() {
    auto& bus = GetEventBus();
    bus.DispatchBeginFrame();
    bus.ClearEvents();

    Sort();

    for (auto& simulator : simulators) {
        simulator->BeginFrameSimulation(bodies);
    }
}

void PhysicDomain::EndFrameDomain() {
    Sort();

    for (auto& simulator : simulators) {
        simulator->EndFrameSimulation(bodies);
    }

    GetEventBus().DispatchEndFrame();
}

void PhysicDomain::DestroyDomain() {
    Sort();

    for (auto& simulator : simulators) {
        simulator->DestroySimulation();
    }
    GetEventBus().ClearAll();
}

void PhysicDomain::AddByObject(GameObject* gameObject) {
    if (!gameObject) return;
    Body::PhysicalBody* body = gameObject->GetComponentOfType<Body::PhysicalBody>();
    if (!body) return;
    if (std::find(bodies.begin(), bodies.end(), body) == bodies.end()) bodies.push_back(body);
}

void PhysicDomain::RemoveByObject(GameObject* gameObject) {
    if (!gameObject) return;
    Body::PhysicalBody* body = gameObject->GetComponentOfType<Body::PhysicalBody>();
    if (!body) return;
    auto it = std::find(bodies.begin(), 
                    bodies.end(), body);
    if (it != bodies.end()) {
        bodies.erase(it);
    }
}