#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <type_traits>

#include "Logger/Logger.h"

class Scene;

namespace SerializeField {
    template<typename> inline constexpr bool always_false_v = false;

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

    class ISerializedField {
    public:
        virtual ~ISerializedField() = default;
        virtual std::string ToString() const = 0;
        virtual void FromString(const std::string& str) = 0;
        virtual std::string GetName() const = 0;

        virtual void Resolve(Scene*) {}
    };

    template<typename T>
    class SerializedField : public ISerializedField {
    private:
        T value;
        std::string fieldName;
        
    public:
        SerializedField(const std::string& name, const T& defaultValue = T()) 
            : fieldName(name), value(defaultValue) {}
        
        operator T&() { return value; }
        operator const T&() const { return value; }
        
        const T* operator->() const { return &value; }
        T* operator->() { return &value; }
        
        SerializedField& operator=(const T& newValue) {
            value = newValue;
            return *this;
        }
        
        std::string ToString() const override {
            if constexpr (std::is_same_v<T, std::string>) return value;
            else if constexpr (std::is_same_v<T, bool>) return value ? "true" : "false";
            else if constexpr (HasToString<T>) {
                return value.ToString();
            }
            else if constexpr (HasStdToString<T>) return std::to_string(value);
            else {
                static_assert(always_false_v<T>, "SerializedField<T>: T has no ToString...");
                return "";
            }
        }

        void FromString(const std::string& str) override {
            //TODO trhow errors in level over.
            if constexpr (std::is_same_v<T, std::string>) value = str;
            else if constexpr (std::is_same_v<T, int>) {
                try {
                    value = std::stoi(str);
                }
                catch(...) {
                    Logging::LoggerFacade::Warn("Error in FromString.");
                }
            }
            else if constexpr (std::is_same_v<T, float>) {
                try {
                    value = std::stof(str);
                }
                catch(...) {
                    Logging::LoggerFacade::Warn("Error in FromString.");
                }
            }
            else if constexpr (std::is_same_v<T, double>) {
                try {
                    value = std::stod(str);
                }
                catch(...) {
                    Logging::LoggerFacade::Warn("Error in FromString.");
                }
            }
            else if constexpr (std::is_same_v<T, bool>) {
                if (str == "true" || str == "1") value = true;
                else if (str == "false" || str == "0") value = false;
                else { Logging::LoggerFacade::Warn("Not convertable value."); }
            }
            else if constexpr (HasFromString<T>) {
                value.FromString(str);
            }
            else {
                static_assert(always_false_v<T>, "SerializedField<T>: T has no FromString...");
                return;
            }
        }

        std::string GetName() const override { return fieldName; }
        
        const T& GetValue() const { return value; }
        T& GetValue() { return value; }
    };

    class ISerializable {
    public:
        virtual ~ISerializable() = default;
        virtual std::vector<ISerializedField*> GetSerializedFields() = 0;
        virtual std::vector<const ISerializedField*> GetSerializedFields() const = 0;
    };

    #define FIELD(type, name) \
        SerializedField<type> name {#name};

    #define SERIALIZED_FIELDS_BASE(...) \
        std::vector<ISerializedField*> GetSerializedFields() override { \
            return std::vector<ISerializedField*>{ __VA_ARGS__ }; \
        } \
        std::vector<const ISerializedField*> GetSerializedFields() const override { \
            return std::vector<const ISerializedField*>{ __VA_ARGS__ }; \
        }

    #define SERIALIZED_FIELDS(BASE, ...) \
        std::vector<ISerializedField*> GetSerializedFields() override { \
            auto fields = BASE::GetSerializedFields(); \
            ISerializedField* arr[] = { __VA_ARGS__ }; \
            for (auto* f : arr) fields.push_back(f); \
            return fields; \
        } \
        std::vector<const ISerializedField*> GetSerializedFields() const override { \
            auto fields = BASE::GetSerializedFields(); \
            const ISerializedField* arr[] = { __VA_ARGS__ }; \
            for (auto* f : arr) fields.push_back(f); \
            return fields; \
        }
}