#pragma once

#include "ISystem.h"

#include <string>
#include <unordered_map>

namespace Systems {
    class RequireSystem {
    private:
        std::unordered_map<std::string, ISystem*> nameToSystem;

    public:
        template <typename T>
            requires (NamedSystemType<T>)
        void AddSystemByType() {
            AddSystemByName(GetNameFromType<T>());
        }

        void AddSystemByName(std::string_view systemName) {
            nameToSystem.try_emplace(std::string(systemName), nullptr);
        }

        template <typename T>
            requires (NamedSystemType<T>)
        T* GetSystemByType() const {
            return static_cast<T*>(GetSystemByName(GetNameFromType<T>()));
        }

        ISystem* GetSystemByName(std::string_view systemName) const {
            auto it = nameToSystem.find(std::string(systemName));
            return it != nameToSystem.end() ? it->second : nullptr;
        }

        template <typename T>
            requires (NamedSystemType<T>)
        void RemoveSystemByType() {
            nameToSystem.erase(GetNameFromType<T>());
        }

        void RemoveSystemByName(const std::string& systemName) {
            nameToSystem.erase(systemName);
        }
        
        void InjectSystem(const std::string& systemName, ISystem* system) {
            auto it = nameToSystem.find(systemName);
            if (it != nameToSystem.end()) {
                it->second = system;
            }
        }

        const std::unordered_map<std::string, ISystem*>& GetRequiredSystems() const {
            return nameToSystem;
        }
    };
}