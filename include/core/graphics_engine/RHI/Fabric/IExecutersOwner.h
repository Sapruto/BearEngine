#pragma once

#include <vector>
#include <memory>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Base/Executers/IExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"

namespace FabricRHI {
    class IExecutersOwner {
    public:
        virtual ~IExecutersOwner() = default;

        virtual std::vector<IExecuterRHI*> GetIExecuters() const = 0;
        virtual IExecuterRHI* GetIExecuter(ExecuterTypeRHI type) const = 0;
        virtual IExecuterRHI* GetExecuterRaw(ExecuterTypeRHI type) const = 0;

        virtual void Clear() = 0;
        virtual bool IsEmpty() const = 0;
        virtual BackendType GetBackendType() const = 0;
    };
}