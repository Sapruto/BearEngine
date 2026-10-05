#pragma once

#include <memory>
#include <string>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/IExecutorsOwner.h"

namespace RHI::Base {
    class BaseContextInstanceRHI;
    class BaseDevice;
    class BaseWindow;
    class BaseShader;
}

namespace RHI::Executors {
    class ExecutorQueueRHI;
}

namespace RHI::FabricRHI {
    class CreatorRHI;
}

namespace RHI {
    class RuntimeRHI {
    private:
        RuntimeRHI() = default;
        ~RuntimeRHI();

        std::unique_ptr<FabricRHI::CreatorRHI> creator;

        std::unique_ptr<Base::BaseContextInstanceRHI> context;
        std::unique_ptr<Base::BaseWindow> window;
        std::unique_ptr<Base::BaseDevice> device;
        std::unique_ptr<FabricRHI::IExecutorsOwner> executers;
        std::unique_ptr<Executors::ExecutorQueueRHI> queue;

        bool initialized{false};

    public:
        static RuntimeRHI& GetInstance();

        RuntimeRHI(const RuntimeRHI&) = delete;
        RuntimeRHI& operator=(const RuntimeRHI&) = delete;
        RuntimeRHI(RuntimeRHI&&) = delete;
        RuntimeRHI& operator=(RuntimeRHI&&) = delete;

        bool Initialize(FabricRHI::BackendType type = FabricRHI::BackendType::USE_BASE,
                        const std::string& title = "BearEngine",
                        unsigned int width = 1280,
                        unsigned int height = 720);

        void Shutdown();

        bool IsInitialized() const noexcept { return initialized; }

        Base::BaseContextInstanceRHI* Context() const noexcept { return context.get(); }
        Base::BaseWindow* Window() const noexcept { return window.get(); }
        Base::BaseDevice* Device() const noexcept { return device.get(); }
        FabricRHI::IExecutorsOwner* Executors() const noexcept { return executers.get(); }
        Executors::ExecutorQueueRHI* Queue() const noexcept { return queue.get(); }

        FabricRHI::CreatorRHI& Creator() noexcept { return *creator; }
        const FabricRHI::CreatorRHI& Creator() const noexcept { return *creator; }

        FabricRHI::BackendType Backend() const noexcept;

        std::unique_ptr<Base::BaseShader> CreateShader(const std::string& vertexPath,
                                                       const std::string& fragmentPath);
    };
}