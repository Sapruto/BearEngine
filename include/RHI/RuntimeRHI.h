#pragma once

#include <memory>
#include <string>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/ICreatorRHI.h"
#include "RHI/Fabric/IExecutorsOwner.h"

namespace RHI::Base {
    class BaseContextInstanceRHI;
    class BaseWindow;
    class BaseDevice;
    class BaseShader;
}

namespace RHI::Executors {
    class ExecutorQueueRHI;
}

namespace RHI {
    class RuntimeRHI {
    private:
        std::unique_ptr<FabricRHI::ICreatorRHI> creator;

        std::shared_ptr<Base::BaseContextInstanceRHI> context;
        std::shared_ptr<Base::BaseWindow> window;
        std::shared_ptr<Base::BaseDevice> device;

        std::unique_ptr<FabricRHI::IExecutorsOwner> executers;
        std::unique_ptr<Executors::ExecutorQueueRHI> queue;

        bool initialized = false;

        FabricRHI::BackendType type;

        RuntimeRHI() = default;
        ~RuntimeRHI();

    public:
        static RuntimeRHI& GetInstance();

        RuntimeRHI(const RuntimeRHI&) = delete;
        RuntimeRHI& operator=(const RuntimeRHI&) = delete;
        RuntimeRHI(RuntimeRHI&&) = delete;
        RuntimeRHI& operator=(RuntimeRHI&&) = delete;

        bool Initialize(FabricRHI::BackendType type,
                        const std::string& title = "BearEngine",
                        unsigned int width = 1280,
                        unsigned int height = 720);

        void Shutdown();
        bool IsInitialized() const noexcept { return initialized; }

        FabricRHI::BackendType Backend() const noexcept;

        Base::BaseContextInstanceRHI* Context() const noexcept { return context.get(); }
        Base::BaseWindow* Window() const noexcept { return window.get(); }
        Base::BaseDevice* Device() const noexcept { return device.get(); }
        FabricRHI::IExecutorsOwner* Executors() const noexcept { return executers.get(); }
        Executors::ExecutorQueueRHI* Queue() const noexcept { return queue.get(); }
        FabricRHI::ICreatorRHI* Creator() const noexcept { return creator.get(); }
    };
}