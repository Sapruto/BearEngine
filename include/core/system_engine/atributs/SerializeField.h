#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <type_traits>
#include <memory>

#include "Vector3.h"
#include "Vector2.h"
#include "UIRenderSettings.h"

template<typename T>
concept HasStaticToString = requires(const T& val) {
    { T::ToString(val) } -> std::convertible_to<std::string>;
};

template<typename T>
concept HasStaticFromString = requires(const std::string& str) {
    { T::FromString(str) } -> std::convertible_to<T>;
};

template<typename T>
concept HasToString = requires(const T& val) {
    { val.ToString() } -> std::convertible_to<std::string>;
};

template<typename T>
concept HasFromString = requires(T& val, const std::string& str) {
    { val.FromString(str) };
};

template<typename T>
concept HasStdToString = requires(const T& val) {
    { std::to_string(val) } -> std::convertible_to<std::string>;
};

class SerializedFieldBase {
public:
    virtual ~SerializedFieldBase() = default;
    virtual std::string ToString() const = 0;
    virtual void FromString(const std::string& str) = 0;
    virtual std::string GetName() const = 0;
};

template<typename T>
class SerializedField : public SerializedFieldBase {
private:
    T value;
    std::string fieldName;
    
public:
    SerializedField(const std::string& name, const T& defaultValue = T()) 
        : fieldName(name), value(defaultValue) {}
    
    operator T() const { return value; }
    
    const T* operator->() const { return &value; }
    T* operator->() { return &value; }
    
    SerializedField& operator=(const T& newValue) {
        value = newValue;
        return *this;
    }
    
    std::string ToString() const override {
        if constexpr (std::is_same_v<T, std::string>) return value;
        else if constexpr (std::is_same_v<T, bool>) return value ? "true" : "false";
        else if constexpr (HasStaticToString<T>) {
            return T::ToString(value);
        }
        else if constexpr (HasToString<T>) {
            return value.ToString();
        }
        else if constexpr (HasStdToString<T>) return std::to_string(value);
        else return "";
    }

    void FromString(const std::string& str) override {
        if constexpr (std::is_same_v<T, std::string>) value = str;
        else if constexpr (std::is_same_v<T, int>) value = std::stoi(str);
        else if constexpr (std::is_same_v<T, float>) value = std::stof(str);
        else if constexpr (std::is_same_v<T, double>) value = std::stod(str);
        else if constexpr (std::is_same_v<T, bool>) value = (str == "true" || str == "1");
        else if constexpr (HasStaticFromString<T>) {
            value = T::FromString(str);
        }
        else if constexpr (HasFromString<T>) {
            value.FromString(str);
        }
    }

    std::string GetName() const override { return fieldName; }
    
    const T& GetValue() const { return value; }
    T& GetValue() { return value; }
};

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual std::vector<SerializedFieldBase*> GetSerializedFields() = 0;
    virtual std::vector<const SerializedFieldBase*> GetSerializedFields() const = 0;
};

#define FIELD(type, name) \
    SerializedField<type> name {#name};

#define SERIALIZED_FIELDS_BASE(...) \
    std::vector<SerializedFieldBase*> GetSerializedFields() override { \
        return std::vector<SerializedFieldBase*>{ __VA_ARGS__ }; \
    } \
    std::vector<const SerializedFieldBase*> GetSerializedFields() const override { \
        return std::vector<const SerializedFieldBase*>{ __VA_ARGS__ }; \
    }

#define SERIALIZED_FIELDS(...) \
    std::vector<SerializedFieldBase*> GetSerializedFields() override { \
        auto fields = Component::GetSerializedFields(); \
        for (auto* field : std::vector<SerializedFieldBase*>{ __VA_ARGS__ }) { \
            fields.push_back(field); \
        } \
        return fields; \
    } \
    std::vector<const SerializedFieldBase*> GetSerializedFields() const override { \
        auto fields = Component::GetSerializedFields(); \
        for (auto* field : std::vector<const SerializedFieldBase*>{ __VA_ARGS__ }) { \
            fields.push_back(field); \
        } \
        return fields; \
    }

#define REGISTER_FIELD(field) field {#field}