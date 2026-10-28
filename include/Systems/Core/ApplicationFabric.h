#pragma once

#include "Application.h"

#include "../Input/InputSystem.h"
#include "../Resource/ResourceSystem.h"
#include "../Time/TimeSystem.h"

#include <memory>

class ApplicationFabric {
public:
    static std::unique_ptr<Application> Create() {
        std::unique_ptr<Application> application = std::make_unique<Application>();
        
        application->AddSystem<ResourceSystem>();
        application->AddSystem<TimeSystem>();

        return std::move(application);
    }
};