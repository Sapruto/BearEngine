#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "RHI/Base/Handles.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"

namespace RHI::Executors::ResourceExecutor {
    struct CreateBufferResult {
        Handles::BufferHandle handle{};
    };

    struct UpdateBufferResult {
        Handles::BufferHandle handle{};
        uint64_t bytesWritten = 0;
    };

    struct CopyBufferResult {
        Handles::BufferHandle src{}, dst{};
        uint64_t bytesCopied = 0;
    };

    struct DestroyBufferResult {
        Handles::BufferHandle handle{};
    };

    struct CreateTextureResult {
        Handles::TextureHandle handle{};
    };

    struct UpdateTextureResult {
        Handles::TextureHandle handle{};
        uint64_t bytesWritten = 0;
    };

    struct CopyTextureResult {
        Handles::TextureHandle src{}, dst{};
    };

    struct DestroyTextureResult {
        Handles::TextureHandle handle{};
    };

    struct CreateSamplerResult {
        Handles::SamplerHandle handle{};
    };

    struct DestroySamplerResult {
        Handles::SamplerHandle handle{};
    };

    struct CreateShaderResult {
        Handles::ShaderHandle handle{};
    };

    struct DestroyShaderResult {
        Handles::ShaderHandle handle{};
    };

    struct CreateRenderTargetResult {
        Handles::RenderTargetHandle handle{};
    };

    struct DestroyRenderTargetResult {
        Handles::RenderTargetHandle handle{};
    };

    struct ResourceExecuteResult : public BaseProcessResult {
        enum class OperationType {
            CREATE_BUFFER,
            UPDATE_BUFFER,
            COPY_BUFFER,
            DESTROY_BUFFER,
            CREATE_TEXTURE,
            UPDATE_TEXTURE,
            COPY_TEXTURE,
            DESTROY_TEXTURE,
            CREATE_SAMPLER,
            DESTROY_SAMPLER,
            CREATE_SHADER,
            DESTROY_SHADER,
            CREATE_RENDER_TARGET,
            DESTROY_RENDER_TARGET,
            UNKNOWN
        };

        OperationType operationType{OperationType::UNKNOWN};

        std::variant<
            CreateBufferResult,
            UpdateBufferResult,
            CopyBufferResult,
            DestroyBufferResult,
            CreateTextureResult,
            UpdateTextureResult,
            CopyTextureResult,
            DestroyTextureResult,
            CreateSamplerResult,
            DestroySamplerResult,
            CreateShaderResult,
            DestroyShaderResult,
            CreateRenderTargetResult,
            DestroyRenderTargetResult
        > data;

        ResourceExecuteResult() = default;

        ResourceExecuteResult(const CreateBufferResult& r)
            : operationType(OperationType::CREATE_BUFFER), data(r) {}

        ResourceExecuteResult(const UpdateBufferResult& r)
            : operationType(OperationType::UPDATE_BUFFER), data(r) {}

        ResourceExecuteResult(const CopyBufferResult& r)
            : operationType(OperationType::COPY_BUFFER), data(r) {}

        ResourceExecuteResult(const DestroyBufferResult& r)
            : operationType(OperationType::DESTROY_BUFFER), data(r) {}

        ResourceExecuteResult(const CreateTextureResult& r)
            : operationType(OperationType::CREATE_TEXTURE), data(r) {}

        ResourceExecuteResult(const UpdateTextureResult& r)
            : operationType(OperationType::UPDATE_TEXTURE), data(r) {}

        ResourceExecuteResult(const CopyTextureResult& r)
            : operationType(OperationType::COPY_TEXTURE), data(r) {}

        ResourceExecuteResult(const DestroyTextureResult& r)
            : operationType(OperationType::DESTROY_TEXTURE), data(r) {}

        ResourceExecuteResult(const CreateSamplerResult& r)
            : operationType(OperationType::CREATE_SAMPLER), data(r) {}

        ResourceExecuteResult(const DestroySamplerResult& r)
            : operationType(OperationType::DESTROY_SAMPLER), data(r) {}

        ResourceExecuteResult(const CreateShaderResult& r)
            : operationType(OperationType::CREATE_SHADER), data(r) {}

        ResourceExecuteResult(const DestroyShaderResult& r)
            : operationType(OperationType::DESTROY_SHADER), data(r) {}

        ResourceExecuteResult(const CreateRenderTargetResult& r)
            : operationType(OperationType::CREATE_RENDER_TARGET), data(r) {}

        ResourceExecuteResult(const DestroyRenderTargetResult& r)
            : operationType(OperationType::DESTROY_RENDER_TARGET), data(r) {}
    };
}