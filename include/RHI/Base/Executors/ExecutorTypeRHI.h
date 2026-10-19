#pragma once

namespace RHI::Executors {
    enum class ExecutorTypeRHI {
        Base,
        SwapChainExecutor,
        ResourceExecutor,
        GeometryExecutor,
        DrawExecutor,
        PipelineExecutor
    };
}