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

#include "Domains/Renderer/3D/Utilities/FrustumUtils.h"

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

std::vector<RenderMeshDesc> RendererScene3D::GetDescsByGroup(MaterialType type) {
    std::vector<RenderMeshDesc> descs;

    for (auto renderable : groups[type]) {
        if (!renderable) continue;
        descs.push_back(RenderMeshDesc{ renderable->GetMesh(), renderable->GetMaterialOfType(type), renderable->GetMatrix() });
    }
    return descs;
}

void RendererScene3D::InitDefault(){
    RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    for(auto* irendarable : renderables){
        Renderable3D* renderable3D = dynamic_cast<Renderable3D*>(irendarable);
        if(!renderable3D) continue;

        if(!renderable3D->HasMaterials()) {
            renderable3D->AddMaterial(std::make_unique<DefaultMaterial>());
        }
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
                    if (auto* tex = std::get_if<CreateTextureResult>(&res.data)) {
                        colorTex = tex->handle;
                    }
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
                    if (auto* tex = std::get_if<CreateTextureResult>(&res.data)) {
                        depthTex = tex->handle;
                    }
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

        auto id = resourceExecutor->AddT<CreateRenderTargetParams>(
                CreateRenderTargetParams{ desc })
            .RemoveAfterExecute().MarkProcess().Commit();
        resourceExecutor->SubscribeOnce(id,
            [&](const std::vector<ResourceExecuteResult>& results) {
                for (const auto& res : results) {
                    if (auto* rt = std::get_if<CreateRenderTargetResult>(&res.data)) {
                        renderTarget = rt->handle;
                    }
                }
            });
    }

    queue->Execute();
}

void RendererScene3D::BuildGroups(){
    if (!isDirty) return;

    for(auto& [type, group] : groups){
        group.clear();
    }

    for(auto* irendarable : renderables){
        Renderable3D* renderable3D = dynamic_cast<Renderable3D*>(irendarable);
        if(!renderable3D) continue;

        std::vector<Material*> materials = renderable3D->GetMaterials();

        for(auto* material : materials) {
            groups[material->GetType()].push_back(renderable3D);
        }
    }

    for(auto& [type, meshShadingRenderer] : passes) {
        if (meshShadingRenderer) meshShadingRenderer->Init();
    }

    isDirty = false;
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
    for(auto& [type, meshShadingRenderer] : passes){
        if (meshShadingRenderer) meshShadingRenderer->Init();
    }
    isInitialized = true;
}

void RendererScene3D::EndFrameDomain() {
    if (!isInitialized) return;

    BuildGroups();

    GenerateNewContext();

    BeginRender();
    for(auto& [type, pass] : passes){
        if(!pass || !groups.count(type)) continue;
        std::vector<RenderMeshDesc> descs = GetDescsByGroup(type);

        pass->BeginFrame(context.get());
        pass->DrawFrame(descs);
    }
    Present();
}