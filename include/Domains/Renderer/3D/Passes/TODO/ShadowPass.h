#pragma once

#include "BasePass3D.h"
#include "Renderer/Base/Shader.h"

namespace Renderer::R3D::Passes {
    class ShadowPass : public BasePass3D {
    private:
        RHI::Handles::PipelineHandle handle{};
        RHI::Handles::BufferHandle lightUBO{};
        uint32_t instanceCapacity = 0;

        const std::filesystem::path vertexShaderPath;
        const std::filesystem::path fragmentShaderPath;

        Base::Shader shader;

        struct LightUBOData {
            alignas(64) float lightViewProj[16];
        };

        void CreateShaders();
        void CreateUBOs();
        void CreateInstanceBuffer(uint32_t initialCapacity);
        void EnsureInstanceCapacity(uint32_t needed);
        void CreatePipeline();
        bool ValidateInit() const;

    public:
        ShadowPass();
        ~ShadowPass() override {}

        void Init() override;
        void BeginFrame(RenderContext3D* context) override;
        void DrawFrame(const std::vector<RenderBatch>& batches) override;
        void Shutdown() override;
    };
}