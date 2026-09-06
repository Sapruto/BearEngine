#pragma once

#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"

namespace BufferExecute {
    template<typename BufferExecuterImpl>
    class BufferExecuter : public BaseExecuterRHI<BufferExecuterImpl, BufferParams, BufferExecuteResult> {
    public:
        BufferExecuter() {
            type = ExecuterTypeRHI::BufferExecuter;
        }
    };
}