#pragma once

#include "../Body/PhysicalBody.h"
#include "PhysicEventBus.h"

#include "Systems/Time/TimeSystem.h"

namespace Physic {
    class IPhysicSimulator {
    private:
        unsigned int layer = 0;
        Events::PhysicEventBus& eventBus;
        TimeSystem& timeSystem;

    protected:
        void SetLayer(unsigned int layer) { this->layer = layer; }

    public:
        IPhysicSimulator() = delete;
        virtual ~IPhysicSimulator() = default;

        virtual void StartSimulation() {}
        virtual void BeginFrameSimulation([[maybe_unused]] const std::vector<Body::PhysicalBody*>& bodies) {}
        virtual void EndFrameSimulation([[maybe_unused]] const std::vector<Body::PhysicalBody*>& bodies) {}
        virtual void DestroySimulation() {}

        void SetTimeSystem(TimeSystem& timeSystem_) { timeSystem = timeSystem_; }
        TimeSystem& GetTimeSystem() { return timeSystem; }

        void SetEventBus(Events::PhysicEventBus& eventBus_) { eventBus = eventBus_; }
        Events::PhysicEventBus& GetEventBus() { return eventBus; }

        unsigned int GetLayer() const { return layer; }
    };
}