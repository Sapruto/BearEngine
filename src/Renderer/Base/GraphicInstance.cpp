#include "Renderer/Base/GraphicInstance.h"

#include "RHI/Base/Executors/ExecutorQueueRHI.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/Fabric/FabricTypes.h"
#include "RHI/RuntimeRHI.h"
#include "Renderer/RenderBackendAllias.h"
#include "Logger/Logger.h"

using namespace RHI;
using namespace RHI::FabricRHI;
using namespace RHI::Executors::SwapChainExecutor;
using namespace Renderer::Base;

bool GraphicInstance::Initialize(const GraphicConfig& config) {
    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(config.backend, config.title, config.width, config.height)) return false;

    auto* queue = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());

    auto* window = rhi.Window();
    window->SetTitle(config.title);

    auto* swapChainExec = executors->GetSwapChainExecutor();
    if (!swapChainExec) return false;

    SwapChainCreateParams scp;
    scp.width = config.width;
    scp.height = config.height;
    scp.bufferCount = config.bufferCount;
    scp.presentMode = config.presentMode;
    scp.colorSpace = config.colorSpace;

    swapChainExec->AddT<SwapChainCreateParams>(SwapChainCreateParams{ scp })
        .RemoveAfterExecute().MarkProcess().Commit();
    queue->Execute();
    return true;
}

void GraphicInstance::Shutdown() {
    
}

void GraphicInstance::BeginFrame() {

}

void GraphicInstance::EndFrame() {

}

IRendererScene* GraphicInstance::CreateScene(std::unique_ptr<IRendererScene> scene) {

}

bool GraphicInstance::DeleteScene(IRendererScene* scene) {
    if (!scene) return false;
    
    auto it = std::find_if(scenes.begin(), scenes.end(),
        [scene](const std::unique_ptr<IRendererScene>& e) {
            return e.get() == scene;
        });
    
    if (it != scenes.end()) {
        scenes.erase(it);
    }
    return true;
}