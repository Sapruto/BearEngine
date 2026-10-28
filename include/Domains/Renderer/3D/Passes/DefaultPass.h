#pragma once

#include "RHI/Base/Handles.h"
#include "BasePass3D.h"

#include <vector>
#include <cstdint>

class Mesh;
class RendererScene;
class Transform3D;

class DefaultPass : public BasePass3D {
private:
    RHI::Handles::PipelineHandle handle;
    RHI::Handles::BufferHandle   cameraUBO;
    RHI::Handles::BufferHandle   lightUBO;

    RHI::Handles::BufferHandle instanceVBO;
    uint32_t instanceCapacity = 0;

    struct CameraUBOData {
        alignas(16) float view[16];
        alignas(16) float projection[16];
        alignas(16) float viewPos[4];
    };

    struct LightUBOData {
        alignas(16) float dirDirection[4];
        alignas(16) float dirColor[4];
        alignas(16) float lightParams[4];
    };

    struct InstanceData {
        alignas(16) float model[16];
        alignas(16) float color[4];
    };

    void CreateShaders(RHI::Handles::ShaderHandle& vs, RHI::Handles::ShaderHandle& fs);
    void CreateUBOs();
    void CreateInstanceBuffer(uint32_t initialCapacity);
    void EnsureInstanceCapacity(uint32_t needed);
    void CreatePipeline(RHI::Handles::ShaderHandle vs, RHI::Handles::ShaderHandle fs);
    bool ValidateInit(RHI::Handles::ShaderHandle vs, RHI::Handles::ShaderHandle fs) const;

public:
    DefaultPass() : BasePass3D(MaterialType::DEFAULT, 0) {}
    ~DefaultPass() override {}

    void Init() override;
    void BeginFrame(RenderContext3D* context) override;
    void DrawFrame(const std::vector<RenderMeshDesc>& descs) override;
    void Shutdown() override;
};