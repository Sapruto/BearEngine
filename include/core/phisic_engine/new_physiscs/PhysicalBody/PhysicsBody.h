#pragma once

#include "include/core/math/geometry/colliders/Polygon2D.h"
#include "include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/PhysicFeature.h"
#include "include/core/system_engine/component_system/Component.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>

class PhysicsWorld;

class PhysicalBody : public Component{
private:
    Collider collider;
    
    float mass;
    float volume;

    std::vector<std::unique_ptr<PhysicFeature>> features;

    const PhysicsWorld world;

public:
    PhysicalBody(Collider* collider, float mass, float volume);
    ~PhysicalBody();

    template<typename T, typename... Args>
    T* AddFeature(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        auto feature = std::make_unique<T>(std::forward<Args>(args)...);
        
        T* ptr = feature.get();
        
        features.push_back(std::move(feature));
        
        ptr->Initialize(this);
        
        return ptr;
    }
    
    void RemoveFeature(PhysicFeature* feature);

    template<typename T>
    T* GetFeatureOfType() {
        static std::type_index typeIdx = typeid(T);
        
        for (const auto& feature : features) {
            if (typeid(*feature) == typeid(T)) {
                return static_cast<T*>(feature.get());
            }
        }
        return nullptr;
    }

    void SetPhysicWorld(PhysicsWorld* world){ this->world = world; }
    void ClearWorld(){ this->world = nullptr; }

    float* GetMass(){ return mass; }
    float* GetVolume{ return volume; }
    Collider* GetCollider(){return collider;}
};