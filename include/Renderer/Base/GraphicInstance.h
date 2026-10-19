#pragma once

#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "Renderer/Base/IRendererScene.h"
#include "RHI/Fabric/FabricTypes.h"

#include <vector>
#include <memory>

namespace Renderer::Base {
    struct GraphicConfig {
        RHI::FabricRHI::BackendType backend = RHI::FabricRHI::BackendType::OpenGL;
        uint32_t width = 1280;
        uint32_t height = 720;
        const char* title = "BearEngine";

        int bufferCount = 2;
        RHI::Executors::SwapChainExecutor::PresentMode presentMode = RHI::Executors::SwapChainExecutor::PresentMode::IMMEDIATE;
        RHI::Executors::SwapChainExecutor::ColorSpace colorSpace = RHI::Executors::SwapChainExecutor::ColorSpace::SRGB_NONLINEAR;


        GraphicConfig() = default;
    };

    class GraphicInstance {
    private:
        std::vector<std::unique_ptr<IRendererScene>> scenes;
        bool isInitialize;

        void ChooseBackend(RHI::FabricRHI::BackendType type);

    public:
        GraphicInstance() = default;

        bool Initialize(const GraphicConfig& config = GraphicConfig());
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        IRendererScene* CreateScene(std::unique_ptr<IRendererScene> scene);
        bool DeleteScene(IRendererScene* scene);
    };
}