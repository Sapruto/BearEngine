#pragma once

#include <vector>
#include <algorithm>

#include "../../Core/DomainSystem.h"
#include "../Body/BodyRegistry.h"
#include "../Events/PhysicEventBus.h"
#include "IPhysicSimulator.h"
#include "Systems/Core/HasRequired.h"

namespace Physic {
    class PhysicDomain : public Domains::IDomainSystem, public Systems::HasRequired {
    private:
        std::vector<std::unique_ptr<IPhysicSimulator>> simulators;

        Body::BodyRegistry bodyRegistry;
        Events::PhysicEventBus eventBus;

        bool isStarted = false;

        bool needSort = true;

        void Sort();

    public:
        PhysicDomain();
        ~PhysicDomain() = default;

        void StartDomain() override;
        void BeginFrameDomain() override;
        void EndFrameDomain() override;
        void DestroyDomain() override;

        template<typename T, typename... Args>
        T* AddPhysicSimulator(Args&&... args) {
            static_assert(std::is_base_of_v<IPhysicSimulator, T>);

            auto simulator = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = simulator.get();

            simulators.push_back(std::move(simulator));
            if (isStarted) raw->StartSimulation();
            needSort = true;
            return raw;
        }

        void RemovePhysicSimulator(IPhysicSimulator* simulator) {
            auto it = std::find_if(simulators.begin(), simulators.end(),
                [simulator](const std::unique_ptr<IPhysicSimulator>& ptr) {
                    return ptr.get() == simulator;
                });
            if (it != simulators.end()) {
                if (isStarted) (*it)->DestroySimulation();
                simulators.erase(it);
            }
            needSort = true;
        }

        void AddByObject(GameObject* gameObject) override;
        void RemoveByObject(GameObject* gameObject) override;

        Body::BodyRegistry& GetBodyRegistry() { return bodyRegistry; }
        Events::PhysicEventBus& GetEventBus() { return eventBus; }
    };
}