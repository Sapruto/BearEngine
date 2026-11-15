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

    //TODO [11.11.26 вроде сделано] сделать чтобы во первых детекторы умные были которые хранили в себе алгоритмы и давали наружу типа просто метод для проверки коллизий произвольного IShape* с другим произвольным IShape*. Второе что нужно сделать переделать вот этот домен к тому интерфейсу детектора и дать возможность его сеттать. Третье реализовать 3д декетор коллизий с AABB, SAT. Четвертое переделать Polyhedron на просто глупую информацию, плюс добавить типа шарик и утилиты которые создают удобные штучки для использования вот эти. Пятое интегрировать эту систему с моей физикой.
};