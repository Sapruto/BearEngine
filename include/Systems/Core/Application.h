#pragma once

#include "Logger/Logger.h"
#include "ISystem.h"

#include <memory>
#include <string>
#include <unordered_map>

class Application {
private:
    std::unordered_map<std::string, std::unique_ptr<Systems::ISystem>> systems;
    bool isStart = false;

public:
    Application() = default;

    template <typename T, typename... Args>
        requires (Systems::NamedSystemType<T>)
    T* AddSystem(Args&&... args) {
        if (isStart) {
            Logging::LoggerFacade::Warn("Try AddSystem after start application");
            return nullptr;
        }

        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        const std::string name{ Systems::GetNameFromType<T>() };

        auto [it, inserted] = systems.try_emplace(name, std::move(ptr));
        if (!inserted) {
            if (typeid(*it->second) != typeid(T)) {
                Logging::LoggerFacade::Warn("...");
                return nullptr;
            }
            it->second = std::move(ptr);
        }
        return static_cast<T*>(it->second.get());
    }

    template <typename T>
        requires (Systems::NamedSystemType<T>)
    T* GetSystemOfType() const {
        auto it = systems.find(std::string(Systems::GetNameFromType<T>()));
        return it != systems.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    Systems::ISystem* GetSystemByName(const std::string& name) const {
        auto it = systems.find(name);
        return it != systems.end() ? it->second.get() : nullptr;
    }

    void StartSystems() {
        isStart = true;
        for (auto& [name, system] : systems) {
            system->StartSystem();
        }
    }
    void BeginFrameSystems() {
        for (auto& [name, system] : systems) {
            system->BeginFrameSystem();
        }
    }
    void EndFrameSystems() {
        for (auto& [name, system] : systems) {
            system->EndFrameSystem();
        }
    }
    void DestroySystems() {
        if (!isStart) return;
        for (auto& [name, system] : systems) {
            system->DestroySystem();
        }
    }
};