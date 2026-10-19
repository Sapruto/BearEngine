#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <bit>

#include "RHI/Base/Enums.h"
#include "RHI/Base/Handles.h"

namespace RHI::Executors::PipelineExecutor {
    struct ShaderStageDesc {
        ShaderStage stage = ShaderStage::VERTEX;
        Handles::ShaderHandle shaderHandler{};
        std::string entryPoint = "main";
        bool operator==(const ShaderStageDesc&) const = default;

        ShaderStageDesc() = default;
        ShaderStageDesc(ShaderStage newStage, Handles::ShaderHandle handle) : stage(newStage), shaderHandler(handle) {}
    };

    struct VertexAttributeDesc {
        uint32_t location = 0;
        uint32_t binding = 0;
        uint32_t offset = 0;
        Format format = Format::FLOAT3;
        bool operator==(const VertexAttributeDesc&) const = default;

        VertexAttributeDesc() = default;
        VertexAttributeDesc(uint32_t newLocation, uint32_t newBinding, uint32_t newOffset, Format format) :
        location(newLocation), binding(newBinding), offset(newOffset), format(format) {}
    };

    struct VertexBindingDesc {
        uint32_t binding = 0;
        uint32_t stride = 0;
        bool perInstance = false;
        uint32_t instanceDivisor = 1;
        bool operator==(const VertexBindingDesc&) const = default;
    };

    struct StencilFaceState {
        StencilOp failOp = StencilOp::KEEP;
        StencilOp passOp = StencilOp::KEEP;
        StencilOp depthFailOp = StencilOp::KEEP;
        CompareOp compareOp = CompareOp::ALWAYS;
        bool operator==(const StencilFaceState&) const = default;
    };

    struct BlendAttachmentState {
        bool blendEnable = false;
        BlendFactor srcColorBlendFactor = BlendFactor::ONE;
        BlendFactor dstColorBlendFactor = BlendFactor::ZERO;
        BlendOp colorBlendOp = BlendOp::ADD;
        BlendFactor srcAlphaBlendFactor = BlendFactor::ONE;
        BlendFactor dstAlphaBlendFactor = BlendFactor::ZERO;
        BlendOp alphaBlendOp = BlendOp::ADD;
        ColorWriteMask writeMask = ColorWriteMask::ALL;
        bool operator==(const BlendAttachmentState&) const = default;
    };

    struct UniformBindingDesc {
        uint32_t binding = 0;
        uint32_t set = 0;
        std::string name;
        uint64_t size = 0;
        bool operator==(const UniformBindingDesc&) const = default;
    };

    struct PipelineState {
        std::vector<ShaderStageDesc> stages;

        std::vector<VertexAttributeDesc> vertexAttributes;
        std::vector<VertexBindingDesc> vertexBindings;

        PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
        Format indexFormat = Format::UINT4;

        FillMode fillMode = FillMode::SOLID;
        CullMode cullMode = CullMode::BACK;
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
        float depthBias = 0.0f;
        float depthBiasClamp = 0.0f;
        float slopeScaledDepthBias = 0.0f;
        bool depthClampEnable = false;

        bool depthTestEnable  = true;
        bool depthWriteEnable = true;
        CompareOp depthCompareOp = CompareOp::LESS;
        bool stencilTestEnable = false;
        StencilFaceState stencilFront{};
        StencilFaceState stencilBack{};
        uint32_t stencilReadMask  = 0xFF;
        uint32_t stencilWriteMask = 0xFF;

        std::vector<BlendAttachmentState> blendAttachments;

        SampleCount sampleCount = SampleCount::COUNT_1;
        bool alphaToCoverage = false;
        bool alphaToOne = false;

        std::vector<Format> colorFormats;
        Format depthStencilFormat = Format::FLOAT1;

        std::vector<DynamicState> dynamicStates;

        std::vector<UniformBindingDesc> uniformBindings;

        bool operator==(const PipelineState& o) const {
            return stages == o.stages
                && vertexAttributes == o.vertexAttributes
                && vertexBindings == o.vertexBindings
                && topology == o.topology
                && indexFormat == o.indexFormat
                && fillMode == o.fillMode
                && cullMode == o.cullMode
                && frontFace == o.frontFace
                && depthBias == o.depthBias
                && depthBiasClamp == o.depthBiasClamp
                && slopeScaledDepthBias == o.slopeScaledDepthBias
                && depthClampEnable == o.depthClampEnable
                && depthTestEnable == o.depthTestEnable
                && depthWriteEnable == o.depthWriteEnable
                && depthCompareOp == o.depthCompareOp
                && stencilTestEnable == o.stencilTestEnable
                && stencilFront == o.stencilFront
                && stencilBack == o.stencilBack
                && stencilReadMask == o.stencilReadMask
                && stencilWriteMask == o.stencilWriteMask
                && blendAttachments == o.blendAttachments
                && sampleCount == o.sampleCount
                && alphaToCoverage == o.alphaToCoverage
                && alphaToOne == o.alphaToOne
                && colorFormats == o.colorFormats
                && depthStencilFormat == o.depthStencilFormat
                && dynamicStates == o.dynamicStates;
        }

        uint64_t Hash() const {
            uint64_t h = 0;
            auto mix = [&h](uint64_t v) {
                h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            };

            for (const auto& s : stages) {
                mix(static_cast<uint64_t>(s.stage));
                mix(s.shaderHandler.id);
                for (char c : s.entryPoint) mix(static_cast<uint64_t>(c));
                mix(s.entryPoint.size());
            }

            for (const auto& a : vertexAttributes) {
                mix(a.location);
                mix(a.binding);
                mix(a.offset);
                mix(static_cast<uint64_t>(a.format));
            }

            for (const auto& b : vertexBindings) {
                mix(b.binding);
                mix(b.stride);
                mix(b.perInstance);
                mix(b.instanceDivisor);
            }

            mix(static_cast<uint64_t>(topology));
            mix(static_cast<uint64_t>(indexFormat));
            mix(static_cast<uint64_t>(fillMode));
            mix(static_cast<uint64_t>(cullMode));
            mix(static_cast<uint64_t>(frontFace));

            mix(std::bit_cast<uint64_t>(static_cast<double>(depthBias)));
            mix(std::bit_cast<uint64_t>(static_cast<double>(depthBiasClamp)));
            mix(std::bit_cast<uint64_t>(static_cast<double>(slopeScaledDepthBias)));
            mix(depthClampEnable);

            mix(depthTestEnable);
            mix(depthWriteEnable);
            mix(static_cast<uint64_t>(depthCompareOp));
            mix(stencilTestEnable);

            mix(static_cast<uint64_t>(stencilFront.failOp));
            mix(static_cast<uint64_t>(stencilFront.passOp));
            mix(static_cast<uint64_t>(stencilFront.depthFailOp));
            mix(static_cast<uint64_t>(stencilFront.compareOp));

            mix(static_cast<uint64_t>(stencilBack.failOp));
            mix(static_cast<uint64_t>(stencilBack.passOp));
            mix(static_cast<uint64_t>(stencilBack.depthFailOp));
            mix(static_cast<uint64_t>(stencilBack.compareOp));

            mix(stencilReadMask);
            mix(stencilWriteMask);

            for (const auto& b : blendAttachments) {
                mix(b.blendEnable);
                mix(static_cast<uint64_t>(b.srcColorBlendFactor));
                mix(static_cast<uint64_t>(b.dstColorBlendFactor));
                mix(static_cast<uint64_t>(b.colorBlendOp));
                mix(static_cast<uint64_t>(b.srcAlphaBlendFactor));
                mix(static_cast<uint64_t>(b.dstAlphaBlendFactor));
                mix(static_cast<uint64_t>(b.alphaBlendOp));
                mix(static_cast<uint64_t>(b.writeMask));
            }

            mix(static_cast<uint64_t>(sampleCount));
            mix(alphaToCoverage);
            mix(alphaToOne);

            for (auto f : colorFormats) mix(static_cast<uint64_t>(f));
            mix(static_cast<uint64_t>(depthStencilFormat));

            for (auto d : dynamicStates) mix(static_cast<uint64_t>(d));

            for (const auto& ub : uniformBindings) {
                mix(ub.binding);
                mix(ub.set);
                for (char c : ub.name) mix(static_cast<uint64_t>(c));
                mix(ub.size);
            }

            return h;
        }
    };

    struct CreatePipelineParams {
        PipelineState state;

        CreatePipelineParams() = default;
        CreatePipelineParams(const PipelineState& newState) : state(newState) {}
    };

    struct DestroyPipelineParams {
        Handles::PipelineHandle handle;
    };

    using PipelineParams = std::variant<
        CreatePipelineParams,
        DestroyPipelineParams
    >;
}

template<>
struct std::hash<RHI::Executors::PipelineExecutor::PipelineState> {
    size_t operator()(const RHI::Executors::PipelineExecutor::PipelineState& s) const noexcept {
        return static_cast<size_t>(s.Hash());
    }
};