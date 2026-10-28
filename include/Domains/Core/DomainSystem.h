#pragma once

#include "World/GameObject.h"

namespace Domains {
    class IDomainSystem {
    public:
        virtual ~IDomainSystem() = default;

        virtual void AddByObject(GameObject* gameObject) = 0;
        virtual void RemoveByObject(GameObject* gameObject) = 0;

        virtual void StartDomain() {}
        virtual void BeginFrameDomain() {}
        virtual void EndFrameDomain() {}
        virtual void DestroyDomain() {}
    };
}