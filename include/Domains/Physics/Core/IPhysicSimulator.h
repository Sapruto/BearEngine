#pragma once

#include "../Events//PhysicEventBus.h"
#include "../Body/BodyRegistry.h"

#include "Systems/Time/TimeSystem.h"

#include <cassert>
#include <stdexcept>

namespace Physic {
    class IPhysicSimulator {
    private:
        unsigned int layer = 0;
        Body::BodyRegistry* bodyRegistry;
        Events::PhysicEventBus* eventBus;
        TimeSystem* timeSystem;

    protected:
        void SetLayer(unsigned int layer) { this->layer = layer; }

    public:
        IPhysicSimulator() = default;
        virtual ~IPhysicSimulator() = default;

        virtual void StartSimulation() {}
        virtual void BeginFrameSimulation() {}
        virtual void EndFrameSimulation() {}
        virtual void DestroySimulation() {}

        void SetBodyRegistry(Body::BodyRegistry* bodyRegistry_) { bodyRegistry = bodyRegistry_; }
        Body::BodyRegistry& GetBodyRegistry() {
            assert(bodyRegistry && "IPhysicSimulator: EventBus not set");
            if (!bodyRegistry) throw std::logic_error("IPhysicSimulator: EventBus not set");
            return *bodyRegistry;
        }

        void SetTimeSystem(TimeSystem* timeSystem_) { timeSystem = timeSystem_; }
        TimeSystem& GetTimeSystem() {
            assert(timeSystem && "IPhysicSimulator: TimeSystem not set");
            if (!timeSystem) throw std::logic_error("IPhysicSimulator: TimeSystem not set");
            return *timeSystem;
        }

        void SetEventBus(Events::PhysicEventBus* eventBus_) { eventBus = eventBus_; }
        Events::PhysicEventBus& GetEventBus() {
            assert(eventBus && "IPhysicSimulator: EventBus not set");
            if (!eventBus) throw std::logic_error("IPhysicSimulator: EventBus not set");
            return *eventBus;
        }

        unsigned int GetLayer() const { return layer; }
    };
}