#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/RectRHI.h"
#include "RHI/Base/Handles.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"

namespace RHI::Executors::DrawExecutor {
    struct DrawArraysResult {
        Handles::PipelineHandle pipeline{};
        uint32_t first = 0;
        uint32_t count = 0;
        uint32_t drawnCount = 0;
        bool success = true;
        std::string errorMessage;
    };

    struct DrawElementsResult {
        Handles::PipelineHandle pipeline{};
        uint32_t indexCount = 0;
        uint32_t drawnCount = 0;
        bool isInstanced = false;
        uint32_t instanceCount = 0;
        bool success = true;
        std::string errorMessage;
    };

    struct BeginRenderPassResult {
        Handles::RenderTargetHandle renderTarget{};
        bool success = true;
        std::string errorMessage;
    };

    struct EndRenderPassResult {
        bool success = true;
        std::string errorMessage;
    };

    struct SetViewportResult {
        RectRHI viewport{};
        bool success = true;
        std::string errorMessage;
    };

    struct SetScissorResult {
        RectRHI scissor{};
        bool success = true;
        std::string errorMessage;
    };

    struct SetBlendConstantsResult {
        bool success = true;
        std::string errorMessage;
    };

    struct BindUniformBufferResult {

    };

    struct BindStorageBufferResult {
        
    };

    struct DrawExecuteResult : public BaseProcessResult {
        enum class OperationType {
            DRAW_ARRAYS,
            DRAW_ELEMENTS,
            BEGIN_RENDER_PASS,
            END_RENDER_PASS,
            SET_VIEWPORT,
            SET_SCISSOR,
            SET_BLEND_CONSTANTS,
            BIND_UNIFORM_BUFFER,
            BindStorageBuffer,
            UNKNOWN
        };

        OperationType operationType{OperationType::UNKNOWN};

        std::variant<
            DrawArraysResult,
            DrawElementsResult,
            BeginRenderPassResult,
            EndRenderPassResult,
            SetViewportResult,
            SetScissorResult,
            SetBlendConstantsResult,
            BindUniformBufferResult,
            BindStorageBufferResult
        > data;

        DrawExecuteResult() = default;

        DrawExecuteResult(const DrawArraysResult& r)
            : operationType(OperationType::DRAW_ARRAYS), data(r) {}

        DrawExecuteResult(const DrawElementsResult& r)
            : operationType(OperationType::DRAW_ELEMENTS), data(r) {}

        DrawExecuteResult(const BeginRenderPassResult& r)
            : operationType(OperationType::BEGIN_RENDER_PASS), data(r) {}

        DrawExecuteResult(const EndRenderPassResult& r)
            : operationType(OperationType::END_RENDER_PASS), data(r) {}

        DrawExecuteResult(const SetViewportResult& r)
            : operationType(OperationType::SET_VIEWPORT), data(r) {}

        DrawExecuteResult(const SetScissorResult& r)
            : operationType(OperationType::SET_SCISSOR), data(r) {}

        DrawExecuteResult(const SetBlendConstantsResult& r)
            : operationType(OperationType::SET_BLEND_CONSTANTS), data(r) {}

        DrawExecuteResult(const BindUniformBufferResult& r)
            : operationType(OperationType::BIND_UNIFORM_BUFFER), data(r) {}
            
        DrawExecuteResult(const BindStorageBufferResult& r)
            : operationType(OperationType::BindStorageBuffer), data(r) {}
    };
}