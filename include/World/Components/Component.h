#pragma once

#include <World/Components/uuid.h>
#include "../GameObject.h"
#include "../Scene/Scene.h"

#include "Engine/Serialization/SerializeField.h"

#include <random>

using namespace SerializeField;

class Component : public ISerializable {
private:
    FIELD(std::string, uuid);

protected:
    GameObject* gameObject;

public:
    Component() : gameObject(nullptr) {}
    
    virtual ~Component() {
        if (gameObject) {
            if (Scene* scene = gameObject->GetScene()) {
                scene->GetCatalog().Unregister(this);
            }
        }
    }

    void Initialize() {
        if (uuid.GetValue().empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static uuids::uuid_random_generator uuid_gen(gen);
            
            uuid.GetValue() = uuids::to_string(uuid_gen());
        }

        if (gameObject) {
            if (Scene* scene = gameObject->GetScene()) {
                scene->GetCatalog().Register(this);
            }
        }
    }

    
    void SetGameObject(GameObject* obj) { 
        gameObject = obj; 
    }
    
    void ClearGameObject(){
        gameObject = nullptr;
    }

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update() {}
    virtual void Destroy() {}
    
    bool IsValid() const {
        return gameObject != nullptr;
    }

    void SetUUID(const std::string& str) {
        uuid.GetValue() = str;
    }

    GameObject* GetGameObject() const { 
        return gameObject; 
    }
    const std::string& GetUUID() const { return uuid.GetValue(); }

    std::string ToString() const {
        return this->GetUUID();
    }
    
    void FromString(const std::string& str) {
        SetUUID(str);
    }

    SERIALIZED_FIELDS(&uuid)
};