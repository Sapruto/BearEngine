#include "Domains/Renderer/3D/Scene/RendererScene3D.h"

#include "Domains/Renderer/3D/Materials/DefaultMaterial.h"
#include "Domains/Renderer/3D/Materials/Material.h"
#include "Domains/Renderer/3D/Materials/MaterialType.h"
#include "Domains/Renderer/3D/Passes/BasePass3D.h"
#include "Domains/Renderer/3D/Passes/DefaultPass.h"

#include "Domains/Renderer/3D/Mesh/Renderable3D.h"
#include "Domains/Renderer/3D/Settings/RenderContext3D.h"
#include "Domains/Renderer/3D/Settings/Cameras/Camera3D.h"
#include "Domains/Renderer/3D/Settings/Lights/ILight3D.h"

#include "Domains/Renderer/3D/Utilities/Frustum.h"

#include "Domains/Renderer/RenderBackendAllias.h"

#include "Systems/Time/TimeSystem.h"
#include "RHI/RuntimeRHI.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"
#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"

using namespace RHI;
using namespace RHI::FabricRHI;
using namespace RHI::Executors::ResourceExecutor;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::Executors::SwapChainExecutor;

using namespace Renderer::R3D;
using namespace Renderer::R3D::Passes;
using namespace Renderer::R3D::Materials;
using namespace Renderer::R3D::Utilities;

void RendererScene3D::InitDefault() {
    RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    for (auto* irendarable : renderables) {
        auto* r3d = irendarable;
        if (!r3d) continue;
        if (!r3d->HasMaterials()) r3d->AddMaterial(std::make_unique<DefaultMaterial>());
    }
    isDirty = true;
}

void RendererScene3D::CreateRenderTarget() {
    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Renderer::Base::Backend>*>(rhi.Executors());
    auto* resourceExecutor = executors->GetResourceExecutor();
    auto* window = rhi.Window();

    const uint32_t w = window ? window->GetWidth()  : 1280;
    const uint32_t h = window ? window->GetHeight() : 720;

    Handles::TextureHandle colorTex{};
    Handles::TextureHandle depthTex{};

    {
        TextureDesc desc;
        desc.width = w;
        desc.height = h;
        desc.format = Format::UBYTE4N;
        desc.usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLED;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "SceneColor";

        auto id = resourceExecutor->AddT<CreateTextureParams>(CreateTextureParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        resourceExecutor->SubscribeOnce(id,
            [&](const std::vector<ResourceExecuteResult>& results) {
                for (const auto& res : results) {
                    if (auto* tex = std::get_if<CreateTextureResult>(&res.data)) colorTex = tex->handle;
                }
            });
    }

    {
        TextureDesc desc;
        desc.width = w;
        desc.height = h;
        desc.format = Format::DEPTH24_STENCIL8;
        desc.usage = TextureUsage::DEPTH_STENCIL;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "SceneDepth";

        auto id = resourceExecutor->AddT<CreateTextureParams>(CreateTextureParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        resourceExecutor->SubscribeOnce(id,
            [&](const std::vector<ResourceExecuteResult>& results) {
                for (const auto& res : results) {
                    if (auto* tex = std::get_if<CreateTextureResult>(&res.data)) depthTex = tex->handle;
                }
            });
    }

    queue->Execute();

    {
        RenderTargetDesc desc;
        desc.colorAttachments = { colorTex };
        desc.depthStencilAttachment = depthTex;
        desc.width = w;
        desc.height = h;
        desc.samples = SampleCount::COUNT_1;
        desc.debugName = "SceneRT";

        auto id = resourceExecutor->AddT<CreateRenderTargetParams>(CreateRenderTargetParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        resourceExecutor->SubscribeOnce(id,
            [&](const std::vector<ResourceExecuteResult>& results) {
                for (const auto& res : results) {
                    if (auto* rt = std::get_if<CreateRenderTargetResult>(&res.data)) renderTarget = rt->handle;
                }
            });
    }

    queue->Execute();
}

void RendererScene3D::BuildBatches() {
    if (isDirty) {
        for (auto& [type, vec] : batchesFlat) vec.clear();

        std::unordered_map<MaterialType, std::unordered_map<Mesh*, size_t>> lookup;

        for (auto* irendarable : renderables) {
            auto* r3d = irendarable;
            if (!r3d) continue;

            Mesh* mesh = r3d->GetMesh();
            if (!mesh) continue;

            for (auto& mat : r3d->GetMaterials()) {
                if (!mat) continue;

                const MaterialType t = mat->GetType();
                auto& vec = batchesFlat[t];
                auto& idx = lookup[t];

                auto it = idx.find(mesh);
                size_t i;
                if (it == idx.end()) {
                    i = vec.size();
                    vec.emplace_back(RenderBatch{ mesh });
                    idx.emplace(mesh, i);
                }
                else {
                    i = it->second;
                }
                vec[i].instances.emplace_back(mat.get(), Matrix4x4f{});
            }
        }

        isDirty = false;
    }

    for (auto* irendarable : renderables) {
        Renderable3D* r3d = irendarable;
        if (!r3d) continue;

        Mesh* mesh = r3d->GetMesh();
        if (!mesh) continue;

        const Matrix4x4f model = r3d->GetMatrix();

        for (auto& mat : r3d->GetMaterials()) {
            if (!mat) continue;

            auto typeIt = batchesFlat.find(mat->GetType());
            if (typeIt == batchesFlat.end()) continue;

            for (auto& batch : typeIt->second) {
                if (batch.mesh != mesh) continue;

                for (auto& inst : batch.instances) {
                    if (inst.material == mat.get()) {
                        inst.modelMatrix = model;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void RendererScene3D::GenerateNewContext() {
    if (!context) return;
    if (!currentCamera) return;

    context->view = currentCamera->GetViewMatrix();
    context->projection = currentCamera->GetProjectionMatrix(context->aspect);
    context->viewPosition = currentCamera->GetGlobalPosition();

    Matrix4x4f vp = context->projection * context->view;
    ExtractFrustumPlanes(vp, context->frustumPlanes);

    if (context->viewportWidth == 0 || context->viewportHeight == 0) {
        if (auto* win = RuntimeRHI::GetInstance().Window()) {
            context->SetViewportSize(win->GetWidth(), win->GetHeight());
        }
    }

    context->lights.clear();
    context->lights.reserve(lights.size());
    for (auto* light : lights) {
        if (!light) continue;
        context->lights.push_back(light->GetLightData());
    }

    if (auto time = GetRequire()->GetSystemByType<TimeSystem>()) {
        context->time = time->TimeScale();
    }
}

void RendererScene3D::BeginRender() {
    if (!context) return;
    if (!isInitialized) return;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Renderer::Base::Backend>*>(rhi.Executors());
    auto* drawExecutor = executors->GetDrawExecutor();

    BeginRenderPassParams rp;
    rp.renderTarget = renderTarget;
    rp.renderArea = {
        0, 0,
        static_cast<float>(context->viewportWidth),
        static_cast<float>(context->viewportHeight)
    };
    rp.clearFlags = BeginRenderPassParams::ClearFlags{ true, true, true };
    rp.clearColorR = clearColor[0];
    rp.clearColorG = clearColor[1];
    rp.clearColorB = clearColor[2];
    rp.clearColorA = clearColor[3];
    rp.clearDepth = 1.0f;
    rp.clearStencil = 0;

    drawExecutor->AddT<BeginRenderPassParams>(BeginRenderPassParams{ rp })
        .RemoveAfterExecute().MarkProcess().Commit();

    queue->Execute();
}

void RendererScene3D::Present() {
    if (!context) return;
    if (!isInitialized) return;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Renderer::Base::Backend>*>(rhi.Executors());
    auto* drawExecutor = executors->GetDrawExecutor();
    auto* swapChainExecutor = executors->GetSwapChainExecutor();

    drawExecutor->AddT<EndRenderPassParams>(EndRenderPassParams{})
        .RemoveAfterExecute().MarkProcess().Commit();

    SwapChainPresentParams spp;
    spp.presentMode = PresentMode::FIFO;
    spp.renderTarget = renderTarget;

    swapChainExecutor->AddT<SwapChainPresentParams>(SwapChainPresentParams{ spp })
        .RemoveAfterExecute().MarkProcess().Commit();

    queue->Execute();
}

void RendererScene3D::StartDomain() {
    if (isInitialized) return;

    CreateRenderTarget();
    InitDefault();
    for (auto& [type, pass] : passes) if (pass) pass->Init();
    isInitialized = true;
}

void RendererScene3D::EndFrameDomain() {
    if (!isInitialized) return;

    BuildBatches();
    GenerateNewContext();

    BeginRender();

    for (auto& [type, pass] : passes) {
        if (!pass) continue;
        auto it = batchesFlat.find(type);
        if (it == batchesFlat.end() || it->second.empty()) continue;

        pass->BeginFrame(context.get());
        pass->DrawFrame(it->second);
    }

    Present();
}