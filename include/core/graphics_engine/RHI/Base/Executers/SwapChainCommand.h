#pragma once

#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

namespace SwapChainExecute {
    template<typename SwapChainExecuterImpl>
    class SwapChainExecuter : public BaseExecuterRHI<SwapChainExecuterImpl, SwapChainParams, SwapChainExecuteResult> {
    public:
        SwapChainExecuter() {
            this->type = ExecuterTypeRHI::SwapChainExecuter;
        }
    };
}