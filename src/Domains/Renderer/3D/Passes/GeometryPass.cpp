#include "Domains/Renderer/3D/Passes/TODO/GeometryPass.h"

#include "Logger/Logger.h"
#include "Domains/Renderer/3D/Materials/DefaultMaterial.h"
#include "Domains/Renderer/3D/Settings/RenderContext3D.h"
#include "Domains/Renderer/3D/Utilities/ModelsShaderPaths.h"
#include "Domains/Renderer/Utilities/ToBytes.h"
#include "Domains/Renderer/RenderBackendAllias.h"

#include "RHI/RuntimeRHI.h"
#include "RHI/MacrosRHI.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"
#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"
#include "RHI/Base/Contracts/PipelineExecutorParams.h"
#include "RHI/Base/Contracts/PipelineExecutorResult.h"
#include "RHI/Base/Contracts/DrawExecutorParams.h"

#include <cstdint>
#include <cstring>
#include <algorithm>

using namespace RHI;
using namespace RHI::Base;
using namespace RHI::Handles;
using namespace RHI::Executors;
using namespace RHI::Executors::ResourceExecutor;
using namespace RHI::Executors::PipelineExecutor;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::FabricRHI;

using namespace Renderer::Base;
using namespace Renderer::Utilities;

using namespace Renderer::R3D;
using namespace Renderer::R3D::Passes;
using namespace Renderer::R3D::Materials;
using namespace Renderer::R3D::Lights;

GeometryPass::GeometryPass() : BasePass3D(Materials::MaterialType::DEFAULT, 0), vertexShaderPath(ModelsShaderPaths::Default::Vertex), fragmentShaderPath(ModelsShaderPaths::Default::Fragment) {}

void GeometryPass::CreateShaders() {
    shader.Initialize({ { ShaderStage::VERTEX, vertexShaderPath }, { ShaderStage::FRAGMENT, fragmentShaderPath } });
}

void GeometryPass::CreateUBOs() {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();

    {
        auto id = resourceExecutor->AddT<CreateBufferParams>(
                CreateBufferParams{ BufferDesc{
                    sizeof(CameraUBOData), BufferUsage::UNIFORM, MemoryType::HOST_VISIBLE } })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateBufferResult, r, { cameraUBO = r.handle; });
    }
}

void GeometryPass::CreateRenderTarget() {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();

    auto* window = rhi.Window();
    const uint32_t width = static_cast<uint32_t>(window->GetWidth());
    const uint32_t height = static_cast<uint32_t>(window->GetHeight());

    rtWidth = width;
    rtHeight = height;

    {
        TextureDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = Format::UBYTE4N;
        desc.usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLED;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "GeometryAlbedo";

        auto id = resourceExecutor->AddT<CreateTextureParams>(CreateTextureParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateTextureResult, r, { albedoTexture = r.handle; });
    }

    {
        TextureDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = Format::UBYTE4N;
        desc.usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLED;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "GeometryNormal";

        auto id = resourceExecutor->AddT<CreateTextureParams>(CreateTextureParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateTextureResult, r, { normalTexture = r.handle; });
    }

    {
        TextureDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = Format::DEPTH24_STENCIL8;
        desc.usage = TextureUsage::DEPTH_STENCIL;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "GeometryDepth";

        auto id = resourceExecutor->AddT<CreateTextureParams>(CreateTextureParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateTextureResult, r, { depthTexture = r.handle; });
    }

    queue->Execute();

    {
        RenderTargetDesc desc;
        desc.colorAttachments = { albedoTexture, normalTexture };
        desc.depthStencilAttachment = depthTexture;
        desc.width = width;
        desc.height = height;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "GeometryRT";

        auto id = resourceExecutor->AddT<CreateRenderTargetParams>(CreateRenderTargetParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateRenderTargetResult, r, { renderTarget = r.handle; });
    }

    queue->Execute();
}

void GeometryPass::CreateInstanceBuffer(uint32_t initialCapacity) {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();

    instanceCapacity = initialCapacity;

    auto id = resourceExecutor->AddT<CreateBufferParams>(
            CreateBufferParams{ BufferDesc{
                sizeof(InstanceData) * instanceCapacity,
                BufferUsage::VERTEX,
                MemoryType::HOST_VISIBLE } })
        .RemoveAfterExecute().MarkProcess().Commit();
    RHI_ONCE(resourceExecutor, id, CreateBufferResult, r, { instanceVBO = r.handle; });
}

void GeometryPass::EnsureInstanceCapacity(uint32_t needed) {
    if (needed <= instanceCapacity) return;

    auto& rhi = RuntimeRHI::GetInstance();
    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();

    if (instanceVBO.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(DestroyBufferParams{ instanceVBO })
            .RemoveAfterExecute().MarkProcess().Commit();
        instanceVBO = {};
    }

    CreateInstanceBuffer(needed * 2);
    queue->Execute();
}

void GeometryPass::CreatePipeline() {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* pipelineExecutor = executors->GetPipelineExecutor();

    PipelineState state{};
    state.stages = {
        ShaderStageDesc{ ShaderStage::VERTEX, shader.GetShader(ShaderStage::VERTEX) },
        ShaderStageDesc{ ShaderStage::FRAGMENT, shader.GetShader(ShaderStage::FRAGMENT) }
    };

    state.vertexAttributes = {
        { 0, 0, offsetof(MeshVertex, position), Format::FLOAT3 },
        { 1, 0, offsetof(MeshVertex, normal), Format::FLOAT3 },
        { 2, 0, offsetof(MeshVertex, texCoords), Format::FLOAT2 },
        { 3, 0, offsetof(MeshVertex, tangent), Format::FLOAT3 },
        { 4, 0, offsetof(MeshVertex, bitangent), Format::FLOAT3 },

        { 5, 1, offsetof(InstanceData, model) +  0, Format::FLOAT4 },
        { 6, 1, offsetof(InstanceData, model) + 16, Format::FLOAT4 },
        { 7, 1, offsetof(InstanceData, model) + 32, Format::FLOAT4 },
        { 8, 1, offsetof(InstanceData, model) + 48, Format::FLOAT4 },
        { 9, 1, offsetof(InstanceData, color), Format::FLOAT4 }
    };

    state.vertexBindings = {
        { 0, sizeof(MeshVertex), false, 1 },
        { 1, sizeof(InstanceData), true,  1 }
    };

    state.uniformBindings = {
        { 0, 0, "CameraUBO", sizeof(CameraUBOData) }
    };

    state.topology = PrimitiveTopology::TRIANGLE_LIST;
    state.indexFormat = Format::UINT4;
    state.cullMode = CullMode::BACK;
    state.frontFace = FrontFace::COUNTER_CLOCKWISE;
    state.depthTestEnable = true;
    state.depthWriteEnable = true;
    state.depthCompareOp = CompareOp::LESS;
    state.blendAttachments = { {}, {} };
    state.colorFormats = { Format::UBYTE4N, Format::UBYTE4N };
    state.depthStencilFormat = Format::DEPTH24_STENCIL8;

    const auto id = pipelineExecutor->AddT<CreatePipelineParams>(CreatePipelineParams{ state })
        .RemoveAfterExecute().MarkProcess().Commit();
    RHI_ONCE(pipelineExecutor, id, CreatePipelineResult, r, { handle = r.handle; });
}

bool GeometryPass::ValidateInit() const {
    if (handle.id == 0) { Logging::LoggerFacade::Error("pipeline failed\n"); return false; }
    if (cameraUBO.id == 0){ Logging::LoggerFacade::Error("cameraUBO failed\n"); return false; }
    if (instanceVBO.id == 0) { Logging::LoggerFacade::Error("instanceVBO failed\n"); return false; }
    if (renderTarget.id == 0) { Logging::LoggerFacade::Error("renderTarget failed\n"); return false; }
    if (albedoTexture.id == 0) { Logging::LoggerFacade::Error("albedoTexture failed\n"); return false; }
    if (normalTexture.id == 0) { Logging::LoggerFacade::Error("normalTexture failed\n"); return false; }
    if (depthTexture.id == 0) { Logging::LoggerFacade::Error("depthTexture failed\n"); return false; }
    return true;
}

void GeometryPass::Init() {
    if (isInitialized) return;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* queue = rhi.Queue();

    CreateShaders();
    CreateUBOs();
    CreateRenderTarget();
    CreateInstanceBuffer(256);
    CreatePipeline();

    queue->Execute();

    if (!ValidateInit()) {
        Logging::LoggerFacade::Error("GeometryPass::Init failed\n");
        return;
    }
    isInitialized = true;
}

void GeometryPass::BeginFrame(RenderContext3D* context) {
    if (!isInitialized || !context) return;

    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();
    auto* queue = rhi.Queue();

    {
        CameraUBOData data{};
        std::copy_n(context->view.GetData(), 16, data.view);
        std::copy_n(context->projection.GetData(), 16, data.projection);
        data.viewPos[0] = context->viewPosition.x;
        data.viewPos[1] = context->viewPosition.y;
        data.viewPos[2] = context->viewPosition.z;
        data.viewPos[3] = 1.0f;

        resourceExecutor->AddT<UpdateBufferParams>(
                UpdateBufferParams{ cameraUBO, 0, ToBytes(data) })
            .RemoveAfterExecute().MarkProcess().Commit();
    }

    context->SetTexture(ResourceNames::Albedo, albedoTexture);
    context->SetTexture(ResourceNames::Depth, depthTexture);
    context->SetTexture(ResourceNames::Normal, normalTexture);

    queue->Execute();
}

void GeometryPass::DrawFrame(const std::vector<RenderBatch>& batches) {
    if (!isInitialized || batches.empty()) return;

    auto& rhi = RuntimeRHI::GetInstance();
    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();
    auto* drawExecutor = executors->GetDrawExecutor();

    {
        BeginRenderPassParams rp{};
        rp.target = renderTarget;
        rp.renderArea = { 0.0f, 0.0f,
            static_cast<float>(rtWidth), static_cast<float>(rtHeight) };
        rp.clearFlags = BeginRenderPassParams::ClearFlags{ true, true, true };
        rp.clearColorR = 0.0f;
        rp.clearColorG = 0.0f;
        rp.clearColorB = 0.0f;
        rp.clearColorA = 0.0f;
        rp.clearDepth = 1.0f;
        rp.clearStencil = 0;

        drawExecutor->AddT<BeginRenderPassParams>(BeginRenderPassParams{ rp })
            .RemoveAfterExecute().MarkProcess().Commit();

        drawExecutor->AddT<BindUniformBufferParams>(
                BindUniformBufferParams{ cameraUBO, 0, 0, 0, sizeof(CameraUBOData) })
            .RemoveAfterExecute().MarkProcess().Commit();

        queue->Execute();
    }

    size_t maxGroupSize = 0;
    for (const auto& b : batches) {
        maxGroupSize = std::max(maxGroupSize, b.instances.size());
    }
    EnsureInstanceCapacity(static_cast<uint32_t>(maxGroupSize));
    queue->Execute();

    for (const auto& b : batches) {
        if (!b.mesh || b.instances.empty()) continue;

        std::vector<InstanceData> instances;
        instances.reserve(b.instances.size());

        for (const auto& inst : b.instances) {
            auto* mat = static_cast<DefaultMaterial*>(inst.material);
            const Vector3f c = mat ? mat->GetColor() : Vector3f{ 1.0f, 1.0f, 1.0f };

            InstanceData d{};
            std::copy_n(inst.modelMatrix.GetData(), 16, d.model);
            d.color[0] = c.x;
            d.color[1] = c.y;
            d.color[2] = c.z;
            d.color[3] = 1.0f;
            instances.push_back(d);
        }

        std::vector<std::byte> raw(instances.size() * sizeof(InstanceData));
        std::memcpy(raw.data(), instances.data(), raw.size());

        resourceExecutor->AddT<UpdateBufferParams>(
                UpdateBufferParams{ instanceVBO, 0, std::move(raw) })
            .RemoveAfterExecute().MarkProcess().Commit();

        queue->Execute();

        vbScratch[0] = VertexBufferBinding{ b.mesh->GetVBO(), 0, 0 };
        vbScratch[1] = VertexBufferBinding{ instanceVBO, 1, 0 };

        DrawElementsParams dep{};
        dep.pipeline = handle;
        dep.vertexBuffers = vbScratch.data();
        dep.vertexBufferCount = 2;
        dep.indexBuffer = b.mesh->GetIBO();
        dep.indexBufferOffset = 0;
        dep.data = DrawElementsParams::Instanced{
            static_cast<uint32_t>(b.mesh->GetIndices().size()),
            static_cast<uint32_t>(b.instances.size()),
            0, 0, 0
        };

        drawExecutor->AddT<DrawElementsParams>(DrawElementsParams{ dep })
            .RemoveAfterExecute().MarkProcess().Commit();

        queue->Execute();
    }

    {
        EndRenderPassParams erp{};
        erp.target = renderTarget;

        drawExecutor->AddT<EndRenderPassParams>(EndRenderPassParams{ erp })
            .RemoveAfterExecute().MarkProcess().Commit();

        queue->Execute();
    }
}

void GeometryPass::Shutdown() {
    if (!isInitialized) return;

    auto& rhi = RuntimeRHI::GetInstance();
    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();
    auto* pipelineExecutor = executors->GetPipelineExecutor();

    if (handle.id != 0) {
        pipelineExecutor->AddT<DestroyPipelineParams>(DestroyPipelineParams{ handle })
            .RemoveAfterExecute().MarkProcess().Commit();
        handle = {};
    }
    if (cameraUBO.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(DestroyBufferParams{ cameraUBO })
            .RemoveAfterExecute().MarkProcess().Commit();
        cameraUBO = {};
    }
    if (instanceVBO.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(DestroyBufferParams{ instanceVBO })
            .RemoveAfterExecute().MarkProcess().Commit();
        instanceVBO = {};
    }
    if (renderTarget.id != 0) {
        resourceExecutor->AddT<DestroyRenderTargetParams>(DestroyRenderTargetParams{ renderTarget })
            .RemoveAfterExecute().MarkProcess().Commit();
        renderTarget = {};
    }
    if (albedoTexture.id != 0) {
        resourceExecutor->AddT<DestroyTextureParams>(DestroyTextureParams{ albedoTexture })
            .RemoveAfterExecute().MarkProcess().Commit();
        albedoTexture = {};
    }
    if (normalTexture.id != 0) {
        resourceExecutor->AddT<DestroyTextureParams>(DestroyTextureParams{ normalTexture })
            .RemoveAfterExecute().MarkProcess().Commit();
        normalTexture = {};
    }
    if (depthTexture.id != 0) {
        resourceExecutor->AddT<DestroyTextureParams>(DestroyTextureParams{ depthTexture })
            .RemoveAfterExecute().MarkProcess().Commit();
        depthTexture = {};
    }
    queue->Execute();

    shader.Shutdown();
    isInitialized = false;
}