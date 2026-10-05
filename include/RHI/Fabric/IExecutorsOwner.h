#pragma once

#include <vector>
#include <memory>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Base/Executors/IExecutorRHI.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"

namespace RHI::FabricRHI {
    using RHI::Executors::IExecutorRHI;
    using RHI::Executors::ExecutorTypeRHI;

    class IExecutorsOwner {
    public:
        virtual ~IExecutorsOwner() = default;

        virtual std::vector<IExecutorRHI*> GetIExecutors() const = 0;
        virtual IExecutorRHI* GetIExecutor(ExecutorTypeRHI type) const = 0;
        virtual IExecutorRHI* GetExecutorRaw(ExecutorTypeRHI type) const = 0;

        virtual void Clear() = 0;
        virtual bool IsEmpty() const = 0;
        virtual BackendType GetBackendType() const = 0;
    };
}