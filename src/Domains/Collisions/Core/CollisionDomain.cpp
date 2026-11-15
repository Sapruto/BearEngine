#include "Domains/Collisions/Core/CollisionDomain.h"
#include "Domains/Collisions/3D/CollisionDetector3D.h"

CollisionDomain::CollisionDomain()
    : eventBus(registry)
{
    detector = std::make_unique<CollisionDetector3D>(registry);
}

void CollisionDomain::BeginFrameDomain() {
    const auto& shapes = registry.GetAll();
    for (size_t i = 0; i < shapes.size(); i++)
        for (size_t j = i + 1; j < shapes.size(); j++) {
            ShapeId a = shapes[i]->GetId();
            ShapeId b = shapes[j]->GetId();
            if (detector->HasCollision(a, b))
                eventBus.Report(a, b);
        }
    eventBus.Flush();
}

void CollisionDomain::EndFrameDomain() {
    detector->ClearInfo();
}

void CollisionDomain::AddByObject(GameObject* gameObject) {
    if (!gameObject) return;

    IShape* shape = gameObject->GetComponentDerivedByType<IShape>();
    if (!shape) return;
    
    registry.Register(shape);
}

void CollisionDomain::RemoveByObject(GameObject* gameObject) {
    if (!gameObject) return;

    IShape* shape = gameObject->GetComponentDerivedByType<IShape>();
    if (!shape) return;
    
    registry.Unregister(shape);
}