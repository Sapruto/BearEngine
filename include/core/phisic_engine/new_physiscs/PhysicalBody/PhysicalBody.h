#pragma once

#include "Transform3D.h"
#include "BaseCollider.h"
#include "Component.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>

class PhysicFeature;

class PhysicsWorld;

class PhysicalBody : public Component{
private:
    BaseCollider* collider;

    Transform3D* transform; 
    
    float mass;
    float volume;
    float density;

    std::vector<std::unique_ptr<PhysicFeature>> features;
    std::vector<PhysicFeature*> sortedFeatures;
    bool needsResort = true; 

    const PhysicsWorld* world;

    bool isIntialized = false;
    bool isDestroyed = false;
    bool isValid;

    void Resort();

public:
    PhysicalBody() = default;
    PhysicalBody(BaseCollider* collider, float density);
    PhysicalBody(BaseCollider* collider);
    PhysicalBody(BaseCollider* collider, float mass, float volume);
    ~PhysicalBody();

    void Initialize();
    void PhysicsUpdate();
    
    void Destroy();

    template<typename T, typename... Args>
    T* AddFeature(Args&&... args) {
        static_assert(std::is_base_of<PhysicFeature, T>::value, "T must be derived from PhysicFeature");
        
        auto feature = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = feature.get();
        
        features.push_back(std::move(feature));
        
        ptr->SetBody(this);
        if(isIntialized) {
            ptr->Initialize();
            ptr->FeatureInitialize();
        }
        
        needsResort = true;
        
        return ptr;
    }
    
    template<typename T, typename... Args>
    T* AddFeatureWithLayer(int layer, Args&&... args) {
        static_assert(std::is_base_of<PhysicFeature, T>::value, "T must be derived from PhysicFeature");
        
        auto feature = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = feature.get();
        
        features.push_back(std::move(feature));
        
        ptr->SetBody(this);
        ptr->SetLayer(layer); 
        
        if(isIntialized) {
            ptr->Initialize();
            ptr->FeatureInitialize();
        }
        
        needsResort = true; 
        
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

    void SetDensity(float density){ this->density = density; Initialize(); }
    void SetMass(float mass) { this->mass = mass; }
    void SetPhysicWorld(PhysicsWorld* world){ this->world = world; }
    void ClearWorld(){ this->world = nullptr; }

    float GetMass(){ return mass; }
    float GetDensity(){ return density; }
    float GetVolume(){ return volume; }
    BaseCollider* GetBaseCollider(){return collider;}
    Transform3D* GetTransform() { return transform; }
    const PhysicsWorld* GetWorld() { return world; }

    std::vector<PhysicFeature*> GetFeatures() {
        std::vector<PhysicFeature*> result;
        result.reserve(features.size());
        for (const auto& feature : features) {
            result.push_back(feature.get());
        }
        return result;
    }
};