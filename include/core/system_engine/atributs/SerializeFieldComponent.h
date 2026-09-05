#pragma once

#include <string>
#include <functional>
#include <type_traits>
#include "SerializeField.h"

class SerializedFieldComponentBase : public SerializedFieldBase {
public:
    virtual void Resolve(Scene* scene) = 0;
    virtual ~SerializedFieldComponentBase() = default;
};

template<typename T>
class SerializedFieldComponent : public SerializedFieldComponentBase {
private:
    T value;
    std::string fieldName;
    std::string cachedUUID;
    
public:
    SerializedFieldComponent(const std::string& name, const T& defaultValue = T()) 
        : fieldName(name), value(defaultValue) {}
    
    operator T() const { return value; }
    
    auto operator->() const {
        if constexpr (std::is_pointer_v<T>) {
            return value;
        } else {
            return &value;
        }
    }
    
    SerializedFieldComponent& operator=(const T& newValue) {
        value = newValue;
        if constexpr (std::is_pointer_v<T>) {
            if (value) {
                cachedUUID = value->GetUUID();
            }
        } else {
            cachedUUID = value.GetUUID();
        }
        return *this;
    }
    
    std::string ToString() const override {
        if constexpr (std::is_pointer_v<T>) {
            if (value) {
                return value->GetUUID();
            }
        } else {
            return value.GetUUID();
        }
        return cachedUUID.empty() ? "null" : cachedUUID;
    }
    
    void FromString(const std::string& str) override {
        cachedUUID = str;
        if constexpr (std::is_pointer_v<T>) {
            value = nullptr;
        }
    }
    
    std::string GetName() const override { return fieldName; }
    
    void Resolve(Scene* scene) override {
        if (!cachedUUID.empty() && cachedUUID != "null") {
            using BaseType = typename std::remove_pointer<T>::type;
            auto* resolved = scene->FindComponentByUUID<BaseType>(cachedUUID);
            if constexpr (std::is_pointer_v<T>) {
                value = resolved;
            } 
            else {
                if (resolved) value = *resolved;
            }
        }
    }
    
    const T& GetValue() const { return value; }
    T& GetValue() { return value; }
};

#define FIELD_COMPONENT(type, name) \
    SerializedFieldComponent<type> name {#name};