#pragma once

#include "Domains/Core/DomainRoot.h"
#include "Scene.h"
#include "Logger/Logger.h"

#include <vector>
#include <memory>

#include "../Injectors/ComponentInjector.h"

class World;

class Theater {
private:
    std::vector<std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;

    bool isChangedScene = false;

    Domains::DomainRoot* domainRoot;

    friend class World;
    void WorldSeeYouChange() { isChangedScene = false; }
    void SetDomainRoot(Domains::DomainRoot* domainRoot) { this->domainRoot = domainRoot; }
    
public:
    Theater() = default;
    ~Theater() = default;

    Scene* AddScene(std::unique_ptr<Scene> scene) {
        scenes.push_back(std::move(scene));

        if (domainRoot) {
            scenes[scenes.size() - 1]->GetEventBus().AddSceneEvent(std::make_unique<ComponentInjectorEvent>(*domainRoot));
        }
        else {
            Logging::LoggerFacade::Fatal("No domain root");
        }

        return scenes[scenes.size() - 1].get();
    }

    void RemoveScene(Scene* scene) {
        if (scene == currentScene) {
            Logging::LoggerFacade::Warn("Try remove scene which is current now");
            return;
        }

        std::erase_if(scenes, [scene](const std::unique_ptr<Scene>& p) {
            return p.get() == scene;
        });
    }

    void ChangeCurrentScene(Scene* scene) {
        if (!scene) return;

        auto it = std::find_if(scenes.begin(), scenes.end(),
            [scene](const std::unique_ptr<Scene>& p) { return p.get() == scene; });

        if (it != scenes.end()) {
            currentScene = scene;
            isChangedScene = true;
        }
    }

    Scene* GetCurrentScene() { return currentScene; }
    bool IsChangedScene() const { return isChangedScene; }

    Domains::DomainRoot* GetDomainRoot() { return domainRoot; }
};