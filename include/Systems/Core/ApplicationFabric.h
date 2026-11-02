#pragma once

#include "Application.h"

#include "../Input/InputSystem.h"
#include "../Resource/ResourceSystem.h"
#include "../Time/TimeSystem.h"

#include <memory>

class ApplicationFabric {
public:
    static void SetupApplication(Application* application) {
        if (!application) return;

        application->AddSystem<ResourceSystem>();
        application->AddSystem<TimeSystem>();
    }

    static std::unique_ptr<Application> Create() {
        std::unique_ptr<Application> application = std::make_unique<Application>();
        
        ApplicationFabric::SetupApplication(application.get());

        return std::move(application);
    }
};