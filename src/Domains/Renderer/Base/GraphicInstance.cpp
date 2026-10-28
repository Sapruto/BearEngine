#include "Domains/Renderer/Base/GraphicInstance.h"

#include "RHI/Base/Executors/ExecutorQueueRHI.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/RuntimeRHI.h"
#include "Domains/Renderer/RenderBackendAllias.h"

using namespace RHI;
using namespace RHI::FabricRHI;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::Executors::SwapChainExecutor;
using namespace Renderer::Base;

bool GraphicInstance::Initialize(const GraphicConfig& _config) {
    config = _config;

    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(_config.backend, _config.title, _config.width, _config.height)) return false;

    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());

    auto* window = rhi.Window();
    window->SetTitle(_config.title);

    auto* swapChainExec = executors->GetSwapChainExecutor();
    if (!swapChainExec) return false;

    SwapChainCreateParams scp;
    scp.width = _config.width;
    scp.height = _config.height;
    scp.bufferCount = _config.bufferCount;
    scp.presentMode = _config.presentMode;
    scp.colorSpace = _config.colorSpace;

    swapChainExec->AddT<SwapChainCreateParams>(SwapChainCreateParams{ scp })
        .RemoveAfterExecute().MarkProcess().Commit();
    queue->Execute();

    isInitialized = true;
    return true;
}

void GraphicInstance::Shutdown() {
    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    rhi.Shutdown();
}

RHI::Base::BaseWindow* GraphicInstance::GetWindow() const {
    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    return rhi.Window();
}