#pragma once

namespace RHI::Executors {
    enum class ExecutorTypeRHI {
        Base,
        SwapChainExecutor,
        BufferExecutor,
        GeometryExecutor,
        DrawExecutor
    };
}