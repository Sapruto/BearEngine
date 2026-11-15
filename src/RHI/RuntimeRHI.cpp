#include "RHI/RuntimeRHI.h"

#include "RHI/Fabric/MakeRHI.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"

using namespace RHI;

RuntimeRHI& RuntimeRHI::GetInstance() {
    static RuntimeRHI instance;
    return instance;
}

RuntimeRHI::~RuntimeRHI() {
    Shutdown();
}

bool RuntimeRHI::Initialize(FabricRHI::BackendType type,
                            const std::string& title,
                            unsigned int width,
                            unsigned int height) {
    if (initialized) return true;

    type = type;
    creator = FabricRHI::MakeRHI(type);
    if (!creator) return false;

    context = creator->CreateContext();
    if (!context || !context->Initialize()) { Shutdown(); return false; }

    window = creator->CreateWindow(title, width, height);
    if (!window) { Shutdown(); return false; }

    device = creator->CreateDevice(window);
    if (!device || !device->Initialize()) { Shutdown(); return false; }

    executers = creator->CreateExecutors();
    if (!executers || executers->IsEmpty()) { Shutdown(); return false; }

    queue = std::make_unique<Executors::ExecutorQueueRHI>(device);
    for (auto* e : executers->GetIExecutors()) {
        queue->AddExecutor(e);
    }

    initialized = true;
    return true;
}

void RuntimeRHI::Shutdown() {
    if (!initialized && !creator) return;

    if (queue) queue->Clear();
    queue.reset();
    executers.reset();
    device.reset();
    window.reset();
    context.reset();
    creator.reset();

    initialized = false;
}

FabricRHI::BackendType RuntimeRHI::Backend() const noexcept {
    return GetInstance().type;
}