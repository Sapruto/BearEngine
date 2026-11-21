#pragma once

#include "RHI/Base/Handles.h"
#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "BasePass3D.h"

#include "../../Base/Shader.h"

#include <filesystem>
#include <vector>
#include <array>
#include <cstdint>

class Transform3D;

namespace Renderer::R3D::Passes {
    class GeometryPass : public BasePass3D {
    private:
        RHI::Handles::PipelineHandle handle{};
        RHI::Handles::BufferHandle cameraUBO{};
        RHI::Handles::BufferHandle instanceVBO{};
        uint32_t instanceCapacity = 0;

        RHI::Handles::RenderTargetHandle renderTarget{};
        RHI::Handles::TextureHandle albedoTexture{};
        RHI::Handles::TextureHandle normalTexture{};
        RHI::Handles::TextureHandle depthTexture{};

        uint32_t rtWidth = 0;
        uint32_t rtHeight = 0;

        std::array<RHI::Executors::DrawExecutor::VertexBufferBinding, 2> vbScratch{};

        const std::filesystem::path vertexShaderPath;
        const std::filesystem::path fragmentShaderPath;

        Base::Shader shader;

        struct CameraUBOData {
            alignas(16) float view[16];
            alignas(16) float projection[16];
            alignas(16) float viewPos[4];
        };

        struct InstanceData {
            alignas(16) float model[16];
            alignas(16) float color[4];
        };

        void CreateShaders();
        void CreateUBOs();
        void CreateRenderTarget();
        void CreateInstanceBuffer(uint32_t initialCapacity);
        void EnsureInstanceCapacity(uint32_t needed);
        void CreatePipeline();
        bool ValidateInit() const;

    public:
        GeometryPass();
        ~GeometryPass() override {}

        void Init() override;
        void BeginFrame(RenderContext3D* context) override;
        void DrawFrame(const std::vector<RenderBatch>& batches) override;
        void Shutdown() override;
    };
}