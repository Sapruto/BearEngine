#pragma once

#include <vector>
#include <memory>  
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "CollisionEvent.h"
#include "BaseCollider.h"
#include "ICollisionDetector.h"

class CollisionDomain {
private:
    std::vector<BaseCollider*> colliders;
    
    std::unordered_map<BaseCollider*, std::unordered_map<BaseCollider*, bool>> previousCollisions;
    
    std::vector<std::unique_ptr<CollisionEvent>> currentFrameEvents;

    std::unique_ptr<ICollisionDetector> detector;

    void CheckPair(BaseCollider* a, BaseCollider* b);

public:
    CollisionDomain() = default;
    ~CollisionDomain();

    void Initialize();

    void Update();

    void ProcessEvents();

    void AddCollider(BaseCollider* collider);
    
    void RemoveCollider(BaseCollider* collider);

    void CheckCollisions();
    
    std::vector<BaseCollider*> GetCollisionsFor(BaseCollider* collider) const;
    
    bool AreColliding(const BaseCollider* a, const BaseCollider* b) const; 

    std::vector<std::unique_ptr<CollisionEvent>> TakeCollisionEvents() {
        return std::move(currentFrameEvents);
    }
    
    const std::vector<std::unique_ptr<CollisionEvent>>& GetCollisionEvents() const {
        return currentFrameEvents;
    }
    //TODO сделать чтобы во первых детекторы умные были которые хранили в себе алгоритмы и давали наружу типа просто метод для проверки коллизий произвольного BaseCollider* с другим произвольным BaseCollider*. Второе что нужно сделать переделать вот этот домен к тому интерфейсу детектора и дать возможность его сеттать. Третье реализовать 3д декетор коллизий с AABB, SAT. Четвертое переделать Polyhedron на просто глупую информацию, плюс добавить типа шарик и утилиты которые создают удобные штучки для использования вот эти. Пятое интегрировать эту систему с моей физикой.
};