#pragma once

#include <string>
#include <functional>

#include "SerializeField.h"

template<typename T>
class SerializedFieldComponent : public SerializedFieldBase {
private:
    T* value;
    std::string fieldName;
    std::string cachedUUID;
    
public:
    SerializedFieldComponent(const std::string& name, T* defaultValue = nullptr) 
        : fieldName(name), value(defaultValue) {}
    
    operator T*() const { return value; }
    T* operator->() const { return value; }
    
    SerializedFieldComponent& operator=(T* newValue) {
        value = newValue;
        if (value) {
            cachedUUID = value->GetUUID();
        } 
        else {
            cachedUUID.clear();
        }
        return *this;
    }
    
    std::string ToString() const override {
        if (value) {
            return value->GetUUID();
        }
        return cachedUUID.empty() ? "null" : cachedUUID;
    }
    
    void FromString(const std::string& str) override {
        cachedUUID = str;
        value = nullptr;
    }
    
    std::string GetName() const override { return fieldName; }
    
    void Resolve(Scene* scene) {
        if (!cachedUUID.empty() && cachedUUID != "null") {
            value = scene->FindComponentByUUID<T>(cachedUUID);
        }
    }
    
    T* GetValue() const { return value; }
};

#define FIELD_COMPONENT(type, name) \
    SerializedFieldRef<type> name {#name}; \
    static_assert(std::is_base_of_v<Component, type>, "FIELD_COMPONENT can only be used with Component types"); \
    void _register_##name() { RegisterRefField(&name); }