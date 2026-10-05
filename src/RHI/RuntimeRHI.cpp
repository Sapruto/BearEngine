#include "RHI/RuntimeRHI.h"

#include "RHI/Fabric/CreatorRHI.h"
#include "RHI/Base/Core/BaseContextInstanceRHI.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseShader.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"

#include <utility>

using namespace RHI::Base;
using namespace RHI::FabricRHI;

namespace RHI {
    RuntimeRHI& RuntimeRHI::GetInstance() {
        static RuntimeRHI instance;
        return instance;
    }

    RuntimeRHI::~RuntimeRHI() {
        Shutdown();
    }

    bool RuntimeRHI::Initialize(BackendType type,
                                const std::string& title,
                                unsigned int width,
                                unsigned int height) {
        if (initialized) return true;

        creator = std::make_unique<CreatorRHI>(type);

        context = creator->CreateContext();
        if (!context || !context->Initialize()) {
            Shutdown();
            return false;
        }

        window = creator->CreateWindow(title, width, height);
        if (!window) {
            Shutdown();
            return false;
        }

        std::shared_ptr<BaseWindow> windowView(window.get(), [](BaseWindow*) {});
        device = creator->CreateDevice(std::move(windowView));
        if (!device || !device->Initialize()) {
            Shutdown();
            return false;
        }

        executers = creator->CreateExecutors();
        if (!executers || executers->IsEmpty()) {
            Shutdown();
            return false;
        }

        std::shared_ptr<BaseDevice> deviceView(device.get(), [](BaseDevice*) {});
        queue = std::make_unique<Executors::ExecutorQueueRHI>(std::move(deviceView));

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

    BackendType RuntimeRHI::Backend() const noexcept {
        return creator ? creator->GetBackend() : BackendType::USE_BASE;
    }

    std::unique_ptr<BaseShader> RuntimeRHI::CreateShader(const std::string& vertexPath,
                                                        const std::string& fragmentPath) {
        if (!creator) return nullptr;
        return creator->CreateShader(vertexPath, fragmentPath);
    }
}