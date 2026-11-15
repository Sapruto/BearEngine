#pragma once

#include "RHI/Base/Executors/DrawExecutor.h"
#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/Base/Contracts/DrawExecutorResult.h"
#include "RHI/Base/Handles.h"

namespace RHI::FabricRHI {
    struct OpenGLBackend;
}

namespace RHI::OpenGL {
    class ResourceGL;
    struct PipelineGL;
}

namespace RHI::Executors::PipelineExecutor {
    class PipelineExecutorGL;
}

namespace RHI::Executors::DrawExecutor {
    class DrawExecutorGL final : public DrawExecutor<DrawExecutorGL> {
        friend struct RHI::FabricRHI::OpenGLBackend;

    private:
        PipelineExecutor::PipelineExecutorGL* pipelineExecutor = nullptr;
        Handles::PipelineHandle currentPipeline{};

        void SetPipelineExecutor(PipelineExecutor::PipelineExecutorGL* pipeline) {
            pipelineExecutor = pipeline;
        }

        bool ApplyVBB(
            uint32_t vertexBufferCount,
            const VertexBufferBinding* vertexBuffers,
            OpenGL::ResourceGL& resourceGL,
            const OpenGL::PipelineGL* pipeline,
            std::string& errorMsg);

    public:
        DrawExecuteResult ProcessDrawArraysImpl(RHI::Base::BaseDevice& device, const DrawArraysParams& p);
        DrawExecuteResult ProcessDrawElementsImpl(RHI::Base::BaseDevice& device, const DrawElementsParams& p);

        DrawExecuteResult ProcessBeginRenderPassImpl(RHI::Base::BaseDevice& device, const BeginRenderPassParams& p);
        DrawExecuteResult ProcessEndRenderPassImpl(RHI::Base::BaseDevice& device, const EndRenderPassParams& p);

        DrawExecuteResult ProcessSetViewportImpl(RHI::Base::BaseDevice& device, const SetViewportParams& p);
        DrawExecuteResult ProcessSetScissorImpl(RHI::Base::BaseDevice& device, const SetScissorParams& p);
        DrawExecuteResult ProcessSetBlendConstantsImpl(RHI::Base::BaseDevice& device, const SetBlendConstantsParams& p);

        bool IsValid() override;
    };
}