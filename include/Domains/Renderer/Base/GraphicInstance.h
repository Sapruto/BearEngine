#pragma once

#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Fabric/FabricTypes.h"

namespace Renderer::Base {
    struct GraphicConfig {
        RHI::FabricRHI::BackendType backend = RHI::FabricRHI::BackendType::OpenGL;
        uint32_t width = 1280;
        uint32_t height = 720;
        const char* title = "BearEngine";

        int bufferCount = 2;
        RHI::Executors::SwapChainExecutor::PresentMode presentMode = RHI::Executors::SwapChainExecutor::PresentMode::IMMEDIATE;
        RHI::Executors::SwapChainExecutor::ColorSpace colorSpace = RHI::Executors::SwapChainExecutor::ColorSpace::SRGB_NONLINEAR;

        float viewportWidth = width;
        float viewportHeight = height;

        GraphicConfig() = default;
    };

    class GraphicInstance {
    private:
        bool isInitialized;

        GraphicConfig config;

    public:
        GraphicInstance() = default;

        bool Initialize(const GraphicConfig& config = GraphicConfig());
        void Shutdown();

        void BeginFrame() {}
        void EndFrame() {}

        RHI::Base::BaseWindow* GetWindow() const;
    };
}