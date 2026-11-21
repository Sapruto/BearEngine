#pragma once

#include <memory>  
#include "../../Core/DomainSystem.h"
#include "ICollisionDetector.h"
#include "ShapeRegistry.h"
#include "CollisionEventBus.h"

class CollisionDomain : public Domains::IDomainSystem {
private:
    ShapeRegistry registry;
    std::unique_ptr<ICollisionDetector> detector;
    CollisionEventBus eventBus;

public:
    CollisionDomain();
    ~CollisionDomain() = default;

    void BeginFrameDomain() override;
    void EndFrameDomain() override;

    void AddByObject(GameObject* gameObject) override;
    void RemoveByObject(GameObject* gameObject) override;

    void AddDetector(std::unique_ptr<ICollisionDetector> detector_) { detector = std::move(detector_); }

    ICollisionDetector* GetDetector() { return detector.get(); }
    const ICollisionDetector* GetDetector() const { return detector.get(); }

    ShapeRegistry& GetRegistry() { return registry; }
    const ShapeRegistry& GetRegistry() const { return registry; }
};