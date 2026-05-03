#pragma once

#include "GameObject.h"
#include "Scene.h"
#include <vector>
#include <uuid.h>
#include <random>

#include "SerializeField.h"

class Component : public ISerializable {
private:
    FIELD(std::string, uuid);

public:
    GameObject* gameObject;

    Component() : gameObject(nullptr) {}
    
    virtual ~Component() {}

    void Initialize() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static uuids::uuid_random_generator uuid_gen(gen);
        
        uuid.GetValue() = uuids::to_string(uuid_gen());

        Scene* scene = const_cast<Scene*>(gameObject->GetScene());
        if(scene) scene->RegisterComponent(this);
    }

    
    void SetGameObject(GameObject* obj) { 
        gameObject = obj; 
    }
    
    void ClearGameObject(){
        gameObject = nullptr;
    }
    
    virtual void Start(){}
    
    virtual void Update(){}
    
    virtual void Destroy(){}

    virtual void OnCollisionEnter(GameObject* other){}
    
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

    static Component* FromString(const std::string& str) {
        Component* comp = new Component();
        comp->SetUUID(str);
        return comp;
    }

    static std::string ToString(Component* comp) {
        return comp->GetUUID();
    }

    SERIALIZED_FIELDS_BASE(&uuid)
};