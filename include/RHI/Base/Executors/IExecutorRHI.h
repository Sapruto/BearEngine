#pragma once

#include "RHI/Base/Executors/ExecutorTypeRHI.h"

namespace RHI::Base { class BaseDevice; }

namespace RHI::Executors {
    class IExecutorRHI {
    public:
        virtual ~IExecutorRHI() = default;

        virtual void ProcessParams(RHI::Base::BaseDevice& device) = 0;
        virtual bool IsMarkedToProcess() const = 0;
        virtual void MarkProcess(bool mark) = 0;
        virtual unsigned int GetLayer() const = 0;
        virtual void SetLayer(unsigned int layer) = 0;
        virtual ExecutorTypeRHI GetExecutorType() const = 0;

        virtual bool IsValid() = 0;
    };
}