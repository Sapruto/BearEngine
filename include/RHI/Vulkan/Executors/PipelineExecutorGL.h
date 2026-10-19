#pragma once

#include "RHI/Base/Contracts/PipelineExecutorParams.h"
#include "RHI/Base/Contracts/PipelineExecutorResult.h"
#include "RHI/Base/Executors/PipelineExecutor.h"
#include "RHI/Base/Handles.h"

#include <glad/glad.h>
#include <unordered_map>

namespace RHI::Executors::DrawExecutor {
    class DrawExecutorGL;
}

namespace RHI::Executors::PipelineExecutor {
    struct GLPipeline : Handles::PipelineHandle {
        uint32_t program = 0;
        uint32_t vao = 0;
        PipelineState state;
    };

    class PipelineExecutorGL final : public PipelineExecutor<PipelineExecutorGL> {
    private:
        std::unordered_map<uint64_t, RHI::Handles::PipelineHandle> pipelineCache;

        void SetVertexAttributes(GLuint& vao, const std::vector<VertexAttributeDesc>& attributes, const std::vector<VertexBindingDesc>& bindings);
        void SetRasterizer(CullMode cullMode, FrontFace frontFace, FillMode fillMode,
                            float depthBias, float slopeScaledDepthBias, bool depthClampEnable);
        void SetDepth(bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp);
        void SetStencil(bool stencilTestEnable, uint32_t stencilReadMask, uint32_t stencilWriteMask,
                        StencilFaceState stencilFront, StencilFaceState stencilBake);
        void SetBlend(const std::vector<BlendAttachmentState>& blendAttachments);
        void SetMultisample(bool alphaToCoverage, bool alphaToOne);

        void ApplyPipelineByHandle(RHI::Base::BaseDevice& device, RHI::Handles::PipelineHandle pipelineHandle);

        friend class RHI::Executors::DrawExecutor::DrawExecutorGL;

    public:
        PipelineExecuteResult ProcessCreatePipelineImpl(RHI::Base::BaseDevice& device, const CreatePipelineParams& params);
        PipelineExecuteResult ProcessDestroyPipelineImpl(RHI::Base::BaseDevice& device, const DestroyPipelineParams& params);

        bool IsValid() override;
    };
}