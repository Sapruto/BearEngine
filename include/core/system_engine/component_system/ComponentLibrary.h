#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector>
#include "Component.h"

class ComponentRegistry {
private:
    static inline std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> factories;

    static inline std::unordered_map<std::string, std::string> typeToName;
    
public:
    template<typename T>
    static void Register(const std::string& name) {
        factories[name] = []() -> std::unique_ptr<Component> { 
            return std::make_unique<T>();
        };
        typeToName[typeid(T).name()] = name;
    }
    
    static std::unique_ptr<Component> Create(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    static bool Has(const std::string& name) {
        return factories.find(name) != factories.end();
    }
    
    static std::vector<std::string> GetAllNames() {
        std::vector<std::string> names;
        for (const auto& pair : factories) {
            names.push_back(pair.first);
        }
        return names;
    }

    static std::unique_ptr<Component> CreateComponentByName(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    template<typename T>
    static std::string GetNameByComponent(T* component) {
        std::string typeName = typeid(*component).name();
        auto it = typeToName.find(typeName);
        return it != typeToName.end() ? it->second : "Unknown";
    }
};

namespace AllComponents {
    class Type {
    public:
        template<typename T>
        Type(T*, const char* name) {
            ComponentRegistry::Register<T>(name);
        }
    };
}

#define BEGIN_COMPONENT(ClassType, ...) \
    class ClassType : public __VA_ARGS__ { \
    private: \
        static const bool _registered; \
    public:

#define END_COMPONENT \
    }; \
    const bool ClassType::_registered = []() { \
        ComponentRegistry::Register<ClassType>(#ClassType); \
        return true; \
    }();