#pragma once

#include "Systems/Core/HasRequired.h"
#include "Systems/Core/Application.h"

class SystemInjector {
public:
    void Inject(Application& app, const std::vector<Systems::HasRequired*>& hasRequirers) {
        for (Systems::HasRequired* hasRequire : hasRequirers) {
            if (auto* req = hasRequire->GetRequire()) {
                for (const auto& [name, _] : req->GetRequiredSystems()) {
                    if (auto* sys = app.GetSystemByName(name)) {
                        req->InjectSystem(name, sys);
                    }
                }
            }
        }
    }
};