#pragma once

#include <cstdint>

namespace RHI::Handles {
    struct IHandle {
        uint32_t id = 0;
        bool operator==(const IHandle&) const = default;
    };

    template<typename Tag>
    struct Handle : IHandle {};

    struct BufferTag {};
    struct TextureTag {};
    struct SamplerTag {};
    struct ShaderTag {};
    struct RenderTargetTag {};
    struct PipelineTag {};

    using BufferHandle = Handle<BufferTag>;
    using TextureHandle = Handle<TextureTag>;
    using SamplerHandle = Handle<SamplerTag>;
    using ShaderHandle = Handle<ShaderTag>;
    using RenderTargetHandle = Handle<RenderTargetTag>;
    using PipelineHandle = Handle<PipelineTag>;
}