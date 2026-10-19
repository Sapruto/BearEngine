#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <variant>

#include "RHI/RectRHI.h"
#include "RHI/Base/Handles.h"
#include "RHI/Base/Enums.h"

namespace RHI::Executors::ResourceExecutor {
    enum class BufferUsage : uint32_t {
        NONE = 0,
        VERTEX = 1u << 0,
        INDEX = 1u << 1,
        UNIFORM = 1u << 2,
        STORAGE = 1u << 3,
        INDIRECT = 1u << 4,
        COPY_SRC = 1u << 5,
        COPY_DST = 1u << 6,
        STAGING = 1u << 7,
    };

    constexpr BufferUsage operator|(BufferUsage a, BufferUsage b) noexcept {
        return static_cast<BufferUsage>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr BufferUsage operator&(BufferUsage a, BufferUsage b) noexcept {
        return static_cast<BufferUsage>(
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    enum class MemoryType : uint32_t {
        DEVICE_LOCAL = 1u << 0,
        HOST_VISIBLE = 1u << 1,
        HOST_COHERENT = 1u << 2,
        HOST_CACHED = 1u << 3,
    };

    enum class TextureUsage : uint32_t {
        NONE = 0,
        SAMPLED = 1u << 0,
        STORAGE = 1u << 1,
        COLOR_ATTACHMENT = 1u << 2,
        DEPTH_STENCIL = 1u << 3,
        COPY_SRC = 1u << 4,
        COPY_DST = 1u << 5,
    };

    constexpr TextureUsage operator|(TextureUsage a, TextureUsage b) noexcept {
        return static_cast<TextureUsage>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr TextureUsage operator&(TextureUsage a, TextureUsage b) noexcept {
        return static_cast<TextureUsage>(
            static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr bool HasFlag(TextureUsage v, TextureUsage f) noexcept {
        return (static_cast<uint32_t>(v) & static_cast<uint32_t>(f)) != 0;
    }

    enum class TextureAspect : uint32_t {
        COLOR = 1u << 0,
        DEPTH = 1u << 1,
        STENCIL = 1u << 2,
        ALL = COLOR | DEPTH | STENCIL,
    };

    enum class FilterMode : uint32_t {
        NEAREST,
        LINEAR
    };

    enum class MipFilterMode : uint32_t {
        NEAREST,
        LINEAR
    };

    enum class AddressMode : uint32_t {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRROR_CLAMP_TO_EDGE
    };

    enum class BorderColor : uint32_t {
        TRANSPARENT_BLACK,
        OPAQUE_BLACK,
        OPAQUE_WHITE,
    };

    struct BufferDesc {
        uint64_t size = 0;
        BufferUsage usage = BufferUsage::VERTEX;
        MemoryType memory = MemoryType::DEVICE_LOCAL;
        uint32_t stride = 0;
        bool persistentlyMapped = false;
        std::string debugName;

        BufferDesc() = default;

        BufferDesc(uint64_t newSize,
                    BufferUsage bufferUsage,
                    MemoryType memoryType = MemoryType::DEVICE_LOCAL,
                    uint32_t newStride = 0,
                    bool pm = false,
                    const std::string& dn = {})
            : size(newSize)
            , usage(bufferUsage)
            , memory(memoryType)
            , stride(newStride)
            , persistentlyMapped(pm)
            , debugName(dn) {}
    };

    struct TextureDesc {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t mipLevels = 1;
        uint32_t arrayLayers = 1;
        Format format = Format::FLOAT4;
        TextureUsage usage = TextureUsage::SAMPLED;
        SampleCount samples = SampleCount::COUNT_1;
        std::string debugName;

        TextureDesc() = default;

        TextureDesc(uint32_t w,
                    uint32_t h,
                    Format fmt = Format::FLOAT4,
                    TextureUsage texUsage = TextureUsage::SAMPLED,
                    uint32_t mips = 1,
                    uint32_t layers = 1,
                    uint32_t d = 1,
                    SampleCount sc = SampleCount::COUNT_1,
                    const std::string& dn = {})
            : width(w)
            , height(h)
            , depth(d)
            , mipLevels(mips)
            , arrayLayers(layers)
            , format(fmt)
            , usage(texUsage)
            , samples(sc)
            , debugName(dn) {}
    };

    struct SamplerDesc {
        FilterMode minFilter = FilterMode::LINEAR;
        FilterMode magFilter = FilterMode::LINEAR;
        MipFilterMode mipFilter = MipFilterMode::LINEAR;
        AddressMode addressU = AddressMode::REPEAT;
        AddressMode addressV = AddressMode::REPEAT;
        AddressMode addressW = AddressMode::REPEAT;
        CompareOp compareOp = CompareOp::NEVER;
        float minLod = 0.0f;
        float maxLod = 1000.0f;
        float mipLodBias = 0.0f;
        uint32_t maxAnisotropy = 1;
        BorderColor borderColor = BorderColor::TRANSPARENT_BLACK;
        std::string debugName;

        SamplerDesc() = default;

        SamplerDesc(FilterMode minF,
                    FilterMode magF,
                    MipFilterMode mipF = MipFilterMode::LINEAR,
                    AddressMode addrU = AddressMode::REPEAT,
                    AddressMode addrV = AddressMode::REPEAT,
                    AddressMode addrW = AddressMode::REPEAT,
                    CompareOp cmp = CompareOp::NEVER,
                    uint32_t aniso = 1,
                    BorderColor border = BorderColor::TRANSPARENT_BLACK,
                    const std::string& dn = {})
            : minFilter(minF)
            , magFilter(magF)
            , mipFilter(mipF)
            , addressU(addrU)
            , addressV(addrV)
            , addressW(addrW)
            , compareOp(cmp)
            , maxAnisotropy(aniso)
            , borderColor(border)
            , debugName(dn) {}
    };

    struct ShaderDesc {
        ShaderStage stage = ShaderStage::VERTEX;
        std::vector<std::byte> bytecode;
        std::string entryPoint = "main";
        std::string debugName;

        ShaderDesc() = default;

        ShaderDesc(ShaderStage shaderStage,
                    std::vector<std::byte> code,
                    const std::string& entry = "main",
                    const std::string& dn = {})
            : stage(shaderStage)
            , bytecode(std::move(code))
            , entryPoint(entry)
            , debugName(dn) {}
    };

    struct RenderTargetDesc {
        std::vector<Handles::TextureHandle> colorAttachments;
        std::vector<Handles::TextureHandle> resolveAttachments;
        Handles::TextureHandle depthStencilAttachment{};
        uint32_t width = 0;
        uint32_t height = 0;
        SampleCount samples = SampleCount::COUNT_1;
        std::string debugName;

        RenderTargetDesc() = default;

        RenderTargetDesc(std::vector<Handles::TextureHandle> colors,
                        uint32_t w,
                        uint32_t h,
                        Handles::TextureHandle depth = {},
                        std::vector<Handles::TextureHandle> resolves = {},
                        SampleCount sc = SampleCount::COUNT_1,
                        const std::string& dn = {})
            : colorAttachments(std::move(colors))
            , resolveAttachments(std::move(resolves))
            , depthStencilAttachment(depth)
            , width(w)
            , height(h)
            , samples(sc)
            , debugName(dn) {}
    };

    struct CreateBufferParams {
        BufferDesc desc;

        CreateBufferParams() = default;
        explicit CreateBufferParams(BufferDesc d) : desc(std::move(d)) {}
    };

    struct UpdateBufferParams {
        Handles::BufferHandle handle{};
        uint64_t offset = 0;
        std::vector<std::byte> data;

        UpdateBufferParams() = default;

        UpdateBufferParams(Handles::BufferHandle h,
                        uint64_t off,
                        std::vector<std::byte> d)
            : handle(h)
            , offset(off)
            , data(std::move(d)) {}
    };

    struct CopyBufferParams {
        Handles::BufferHandle src{}, dst{};
        uint64_t srcOffset = 0;
        uint64_t dstOffset = 0;
        uint64_t size = 0;

        CopyBufferParams() = default;

        CopyBufferParams(Handles::BufferHandle s,
                        Handles::BufferHandle d,
                        uint64_t sizeBytes,
                        uint64_t srcOff = 0,
                        uint64_t dstOff = 0)
            : src(s)
            , dst(d)
            , srcOffset(srcOff)
            , dstOffset(dstOff)
            , size(sizeBytes) {}
    };

    struct DestroyBufferParams {
        Handles::BufferHandle handle{};

        DestroyBufferParams() = default;
        explicit DestroyBufferParams(Handles::BufferHandle h) : handle(h) {}
    };

    struct CreateTextureParams {
        TextureDesc desc;

        CreateTextureParams() = default;
        explicit CreateTextureParams(TextureDesc d) : desc(std::move(d)) {}
    };

    struct UpdateTextureParams {
        Handles::TextureHandle handle{};
        uint32_t mipLevel = 0;
        uint32_t arrayLayer = 0;
        RectRHI region{};
        std::vector<std::byte> data;

        UpdateTextureParams() = default;

        UpdateTextureParams(Handles::TextureHandle h,
                            RectRHI r,
                            std::vector<std::byte> d,
                            uint32_t mip = 0,
                            uint32_t layer = 0)
            : handle(h)
            , mipLevel(mip)
            , arrayLayer(layer)
            , region(r)
            , data(std::move(d)) {}
    };

    struct CopyTextureParams {
        Handles::TextureHandle src{}, dst{};
        RectRHI srcRegion{};
        RectRHI dstRegion{};
        TextureAspect aspect = TextureAspect::COLOR;
        FilterMode filter = FilterMode::NEAREST;

        CopyTextureParams() = default;

        CopyTextureParams(Handles::TextureHandle s,
                        Handles::TextureHandle d,
                        RectRHI srcR,
                        RectRHI dstR,
                        TextureAspect a = TextureAspect::COLOR,
                        FilterMode f = FilterMode::NEAREST)
            : src(s)
            , dst(d)
            , srcRegion(srcR)
            , dstRegion(dstR)
            , aspect(a)
            , filter(f) {}
    };

    struct DestroyTextureParams {
        Handles::TextureHandle handle{};

        DestroyTextureParams() = default;
        explicit DestroyTextureParams(Handles::TextureHandle h) : handle(h) {}
    };

    struct CreateSamplerParams {
        SamplerDesc desc;

        CreateSamplerParams() = default;
        explicit CreateSamplerParams(SamplerDesc d) : desc(std::move(d)) {}
    };

    struct DestroySamplerParams {
        Handles::SamplerHandle handle{};

        DestroySamplerParams() = default;
        explicit DestroySamplerParams(Handles::SamplerHandle h) : handle(h) {}
    };

    struct CreateShaderParams {
        ShaderDesc desc;

        CreateShaderParams() = default;
        explicit CreateShaderParams(ShaderDesc d) : desc(std::move(d)) {}
    };

    struct DestroyShaderParams {
        Handles::ShaderHandle handle{};

        DestroyShaderParams() = default;
        explicit DestroyShaderParams(Handles::ShaderHandle h) : handle(h) {}
    };

    struct CreateRenderTargetParams {
        RenderTargetDesc desc;

        CreateRenderTargetParams() = default;
        explicit CreateRenderTargetParams(RenderTargetDesc d) : desc(std::move(d)) {}
    };

    struct DestroyRenderTargetParams {
        Handles::RenderTargetHandle handle{};

        DestroyRenderTargetParams() = default;
        explicit DestroyRenderTargetParams(Handles::RenderTargetHandle h) : handle(h) {}
    };

    using ResourceParams = std::variant<
        CreateBufferParams,
        UpdateBufferParams,
        CopyBufferParams,
        DestroyBufferParams,
        CreateTextureParams,
        UpdateTextureParams,
        CopyTextureParams,
        DestroyTextureParams,
        CreateSamplerParams,
        DestroySamplerParams,
        CreateShaderParams,
        DestroyShaderParams,
        CreateRenderTargetParams,
        DestroyRenderTargetParams
    >;
}