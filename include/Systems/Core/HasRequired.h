#pragma once

#include "RequireSystem.h"

namespace Systems {
    class HasRequired {
    private:
        Systems::RequireSystem requireSystem;

    public:
        HasRequired() = default;
        virtual ~HasRequired() = default;

        Systems::RequireSystem* GetRequire() { return &requireSystem; }
    };
}