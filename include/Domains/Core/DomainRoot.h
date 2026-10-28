#pragma once

#include "DomainSystem.h"
#include "Systems/Core/HasRequired.h"

#include <vector>
#include <memory>

namespace Domains {
    class DomainRoot {
    private:
        std::vector<std::unique_ptr<IDomainSystem>> domainSystems;
        std::vector<Systems::HasRequired*> requirers;

    public:
        DomainRoot() = default;

        template<typename T, typename... Args>
        T* AddDomainSystem(Args&&... args) {
            static_assert(std::is_base_of<IDomainSystem, T>::value, "T must be derived from IDomainSystem");

            auto domainSystem = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = domainSystem.get();
            domainSystems.push_back(std::move(domainSystem));
            return raw;
        }

        void InitializeDomains() {
            requirers.reserve(domainSystems.size());
            for (const auto& domain : domainSystems) {
                if (auto* req = dynamic_cast<Systems::HasRequired*>(domain.get())) {
                    requirers.push_back(req);
                }
            }
        }

        void StartDomains() {
            for (auto& domainSystem : domainSystems) {
                domainSystem->StartDomain();
            }
        }

        void BeginFrameDomains() {
            for (auto& domainSystem : domainSystems) {
                domainSystem->BeginFrameDomain();
            }
        }

        void EndFrameDomains() {
            for (auto& domainSystem : domainSystems) {
                domainSystem->EndFrameDomain();
            }
        }

        void DestroyDomains() {
            for (auto& domainSystem : domainSystems) {
                domainSystem->DestroyDomain();
            }
        }

        const std::vector<std::unique_ptr<IDomainSystem>>& GetAllDomains() const { return domainSystems; }
        const std::vector<Systems::HasRequired*>& GetDomainsWithRequirers() const { return requirers; }
    };
}