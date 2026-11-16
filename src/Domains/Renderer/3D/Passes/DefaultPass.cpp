#include "Domains/Renderer/3D/Passes/DefaultPass.h"

#include "Logger/Logger.h"
#include "Domains/Renderer/3D/Materials/DefaultMaterial.h"
#include "Domains/Renderer/3D/Settings/RenderContext3D.h"
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

constexpr const char kMeshVertexShader[] = R"(
#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 5) in vec4 aModel0;
layout(location = 6) in vec4 aModel1;
layout(location = 7) in vec4 aModel2;
layout(location = 8) in vec4 aModel3;
layout(location = 9) in vec4 aColor;

out vec3 vNormal;
out vec3 vFragPos;
out vec4 vColor;

void main() {
    mat4 model = mat4(aModel0, aModel1, aModel2, aModel3);

    vec4 worldPos = model * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vColor = aColor;

    gl_Position = projection * view * worldPos;
}
)";

constexpr const char kFragmentShader[] = R"(
#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

layout(std140, binding = 2) uniform LightUBO {
    vec4 dirDirection;
    vec4 dirColor;
    vec4 lightParams;
};

in vec3 vNormal;
in vec3 vFragPos;
in vec4 vColor;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos.xyz - vFragPos);

    vec3 result = vec3(0.0);

    if (lightParams.y == 1.0) {
        vec3 L = normalize(-dirDirection.xyz);
        float ndotl = max(dot(N, L), 0.0);
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), 32.0);

        vec3 ambient = 0.1 * dirColor.rgb;
        vec3 diffuse = ndotl * dirColor.rgb;
        vec3 specular = 0.5 * spec * dirColor.rgb;

        result += ambient + diffuse + specular;
    }

 FragColor = vec4(result * vColor.rgb, 1.0);
}
)";

void DefaultPass::CreateShaders(ShaderHandle& vs, ShaderHandle& fs) {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* queue = rhi.Queue();
    auto* resourceExecutor = executors->GetResourceExecutor();

    {
        ShaderDesc desc;
        desc.stage = ShaderStage::VERTEX;
        desc.bytecode = ToBytes(kMeshVertexShader);

        const auto id = resourceExecutor->AddT<CreateShaderParams>(CreateShaderParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        resourceExecutor->SubscribeOnce(id, [&](const std::vector<ResourceExecuteResult>& results) {
            if (results.empty()) return;
            const auto& res = results[0];

            if (auto* shaderRes = std::get_if<CreateShaderResult>(&res.data)) vs = shaderRes->handle;
            else Logging::LoggerFacade::Error("variant does not hold CreateShaderResult\n");
        });
    }
    {
        ShaderDesc desc;
        desc.stage = ShaderStage::FRAGMENT;
        desc.bytecode = ToBytes(kFragmentShader);

        const auto id = resourceExecutor->AddT<CreateShaderParams>(CreateShaderParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateShaderResult, r, { fs = r.handle; });
    }
    queue->Execute();
}

void DefaultPass::CreateUBOs() {
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
    {
        auto id = resourceExecutor->AddT<CreateBufferParams>(
                CreateBufferParams{ BufferDesc{
                    sizeof(LightUBOData), BufferUsage::UNIFORM, MemoryType::HOST_VISIBLE } })
            .RemoveAfterExecute().MarkProcess().Commit();
        RHI_ONCE(resourceExecutor, id, CreateBufferResult, r, { lightUBO = r.handle; });
    }
}

void DefaultPass::CreateInstanceBuffer(uint32_t initialCapacity) {
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

void DefaultPass::EnsureInstanceCapacity(uint32_t needed) {
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

void DefaultPass::CreatePipeline(ShaderHandle vs, ShaderHandle fs) {
    auto& rhi = RuntimeRHI::GetInstance();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* pipelineExecutor = executors->GetPipelineExecutor();

    PipelineState state{};
    state.stages = {
        ShaderStageDesc{ ShaderStage::VERTEX, vs },
        ShaderStageDesc{ ShaderStage::FRAGMENT, fs }
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
        { 0, 0, "CameraUBO", sizeof(CameraUBOData) },
        { 2, 0, "LightUBO",  sizeof(LightUBOData)  }
    };

    state.topology = PrimitiveTopology::TRIANGLE_LIST;
    state.indexFormat = Format::UINT4;
    state.cullMode = CullMode::BACK;
    state.frontFace = FrontFace::COUNTER_CLOCKWISE;
    state.depthTestEnable = true;
    state.depthWriteEnable = true;
    state.depthCompareOp = CompareOp::LESS;
    state.blendAttachments = { {} };
    state.colorFormats = { Format::UBYTE4N };
    state.depthStencilFormat = Format::DEPTH24_STENCIL8;

    const auto id = pipelineExecutor->AddT<CreatePipelineParams>(CreatePipelineParams{ state })
        .RemoveAfterExecute().MarkProcess().Commit();
    RHI_ONCE(pipelineExecutor, id, CreatePipelineResult, r, { handle = r.handle; });
}

bool DefaultPass::ValidateInit(ShaderHandle vs, ShaderHandle fs) const {
    if (vs.id == 0) { Logging::LoggerFacade::Error("vertex shader failed\n"); return false; }
    if (fs.id == 0) { Logging::LoggerFacade::Error("fragment shader failed\n"); return false; }
    if (handle.id == 0) { Logging::LoggerFacade::Error("pipeline failed\n"); return false; }
    if (cameraUBO.id == 0){ Logging::LoggerFacade::Error("cameraUBO failed\n"); return false; }
    if (lightUBO.id == 0) { Logging::LoggerFacade::Error("lightUBO failed\n"); return false; }
    if (instanceVBO.id == 0) { Logging::LoggerFacade::Error("instanceVBO failed\n"); return false; }
    return true;
}

void DefaultPass::Init() {
    if (isInitialized) return;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* queue = rhi.Queue();

    ShaderHandle vsHandle{};
    ShaderHandle fsHandle{};

    CreateShaders(vsHandle, fsHandle);
    CreateUBOs();
    CreateInstanceBuffer(256);
    CreatePipeline(vsHandle, fsHandle);

    queue->Execute();

    if (!ValidateInit(vsHandle, fsHandle)) {
        Logging::LoggerFacade::Error("DefaultPass::Init failed\n");
        return;
    }
    isInitialized = true;
}

void DefaultPass::BeginFrame(RenderContext3D* context) {
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

    {
        LightUBOData data{};
        bool hasDirLight = false;

        for (const auto& light : context->lights) {
            std::visit([&](const auto& l) {
                using T = std::decay_t<decltype(l)>;
                if constexpr (std::is_same_v<T, DirectionalLightData>) {
                    data.dirDirection[0] = l.direction.x;
                    data.dirDirection[1] = l.direction.y;
                    data.dirDirection[2] = l.direction.z;
                    data.dirDirection[3] = 0.0f;

                    data.dirColor[0] = l.color.x * l.intensity;
                    data.dirColor[1] = l.color.y * l.intensity;
                    data.dirColor[2] = l.color.z * l.intensity;
                    data.dirColor[3] = 1.0f;

                    hasDirLight = true;
                }
            }, light);
        }

        data.lightParams[0] = static_cast<float>(context->lights.size());
        data.lightParams[1] = hasDirLight ? 1.0f : 0.0f;
        data.lightParams[2] = 0.0f;
        data.lightParams[3] = 0.0f;

        resourceExecutor->AddT<UpdateBufferParams>(
                UpdateBufferParams{ lightUBO, 0, ToBytes(data) })
            .RemoveAfterExecute().MarkProcess().Commit();
    }

    queue->Execute();
}

void DefaultPass::DrawFrame(const std::vector<RenderBatch>& batches) {
    if (!isInitialized || batches.empty()) return;

    auto& rhi = RuntimeRHI::GetInstance();
    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();
    auto* drawExecutor = executors->GetDrawExecutor();

    drawExecutor->AddT<BindUniformBufferParams>(
            BindUniformBufferParams{ cameraUBO, 0, 0, 0, sizeof(CameraUBOData) })
        .RemoveAfterExecute().MarkProcess().Commit();

    drawExecutor->AddT<BindUniformBufferParams>(
            BindUniformBufferParams{ lightUBO, 2, 0, 0, sizeof(LightUBOData) })
        .RemoveAfterExecute().MarkProcess().Commit();

    queue->Execute();

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
}

void DefaultPass::Shutdown() {
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
    if (lightUBO.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(DestroyBufferParams{ lightUBO })
            .RemoveAfterExecute().MarkProcess().Commit();
        lightUBO = {};
    }
    if (instanceVBO.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(DestroyBufferParams{ instanceVBO })
            .RemoveAfterExecute().MarkProcess().Commit();
        instanceVBO = {};
    }
    queue->Execute();
    isInitialized = false;
}