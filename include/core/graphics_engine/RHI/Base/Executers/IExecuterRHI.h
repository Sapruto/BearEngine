#pragma once

#include "RHI/Base/Executers/ExecuterTypeRHI.h"

class BaseDevice;

class IExecuterRHI {
public:
    virtual ~IExecuterRHI() = default;

    virtual void ProcessParams(BaseDevice& device) = 0;
    virtual bool IsMarkedToProcess() const = 0;
    virtual void MarkProcess(bool mark) = 0;
    virtual unsigned int GetLayer() const = 0;
    virtual void SetLayer(unsigned int layer) = 0;
    virtual ExecuterTypeRHI GetExecuterType() const = 0;
};