#pragma once

#include "Logger/Logger.h"
#include "Systems/Core/Application.h"
#include "Systems/Core/ApplicationFabric.h"
#include "Domains/Core/DomainRoot.h"

#include "Scene/Theater.h"
#include "Systems/Core/HasRequired.h"

#include "Injectors/SystemInjector.h"
#include "Injectors/ComponentInjector.h"

#include <stdexcept>
#include <memory>

class World {
private:
    std::unique_ptr<Application> application;
    std::unique_ptr<Domains::DomainRoot> domainRoot;
    std::unique_ptr<Theater> theater;
    std::unique_ptr<SystemInjector> systemInjector;
    std::unique_ptr<ComponentInjector> componentInjector;

    Scene* GetCurrentScene() {
        Scene* currentScene = theater->GetCurrentScene();
        if (!currentScene) {
            Logging::LoggerFacade::Fatal("No current scene");
            throw std::runtime_error("No current scene");
        }
        return currentScene;
    }

    void InjectSystemsInAllComponents(Scene* currentScene) {
        const std::vector<Systems::HasRequired*>& requireds = currentScene->GetCatalog().ConsumeNewRequirers();
        systemInjector->Inject(*application, requireds);
    }

    void InjectSystemsInDomains() {
        const std::vector<Systems::HasRequired*> requireds = domainRoot->GetDomainsWithRequirers();
        systemInjector->Inject(*application, requireds);
    }

    void InjectComponentsInDomains(Scene* currentScene) {
        componentInjector->InjectAll(*domainRoot, currentScene->GetGameObjects());
    }

public:
    World()
    : domainRoot(std::make_unique<Domains::DomainRoot>()),
    systemInjector(std::make_unique<SystemInjector>()),
    componentInjector(std::make_unique<ComponentInjector>()),
    theater(std::make_unique<Theater>())
    {
        application = ApplicationFabric::Create();
        theater->SetDomainRoot(domainRoot.get());
    }

    void StartWorld() {
        Scene* currentScene = GetCurrentScene();

        application->StartSystems();

        currentScene->InitializeScene();
        InjectSystemsInAllComponents(currentScene);

        currentScene->AwakeScene();

        domainRoot->InitializeDomains();

        InjectSystemsInDomains();
        InjectComponentsInDomains(currentScene);

        domainRoot->StartDomains();

        currentScene->StartScene();
    }

    void UpdateWorld() {
        Scene* currentScene = GetCurrentScene();

        if (theater->IsChangedScene()) {
            theater->WorldSeeYouChange();
            currentScene = theater->GetCurrentScene();

            currentScene->InitializeScene();

            InjectSystemsInAllComponents(currentScene);

            currentScene->AwakeScene();

            InjectComponentsInDomains(currentScene);

            currentScene->StartScene();
        }
        else {
            InjectSystemsInAllComponents(currentScene);
        }

        application->BeginFrameSystems();
        domainRoot->BeginFrameDomains();

        currentScene->UpdateScene();

        domainRoot->EndFrameDomains();
        application->EndFrameSystems();
    }
    
    void DestroyWorld() {
        Scene* currentScene = GetCurrentScene();

        currentScene->DestroyScene();
        domainRoot->DestroyDomains();
        application->DestroySystems();
    }

    Application* GetApplication() const { return application.get(); }
    Domains::DomainRoot* GetDomainRoot() const { return domainRoot.get(); }
    Theater* GetTheater() const { return theater.get(); }
};