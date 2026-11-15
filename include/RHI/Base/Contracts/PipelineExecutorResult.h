#pragma once

#include "RHI/Base/Handles.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"

namespace RHI::Executors::PipelineExecutor {
    struct CreatePipelineResult {
        Handles::PipelineHandle handle;

        CreatePipelineResult() = default;
        CreatePipelineResult(Handles::PipelineHandle newHandle) : handle(newHandle) {}
    };

    struct DestroyPipelineResult {};

    struct PipelineExecuteResult : public BaseProcessResult {
        std::variant<CreatePipelineResult, DestroyPipelineResult> data;

        PipelineExecuteResult() = default;

        PipelineExecuteResult(const CreatePipelineResult& r) : data(r) {}
        PipelineExecuteResult(const DestroyPipelineResult& r) : data(r) {}
    };
}