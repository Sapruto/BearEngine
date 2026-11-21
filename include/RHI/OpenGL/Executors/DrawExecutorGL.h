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
        //TODO сделать чтобы добавляли мы все rt или tex в некоторые списки чтобы можно было отслеживать это коогда закрыавют рендеринг когда его открывают и логировать это все чтобы баги ловились сразу.

        PipelineExecutor::PipelineExecutorGL* pipelineExecutor = nullptr;

        struct DrawResult {
            bool success = true;
            std::string errorMsg = "";

            DrawResult() = default;
            DrawResult(const std::string& errorMsg_) : success(false), errorMsg(errorMsg_) {}
        };

        void SetPipelineExecutor(PipelineExecutor::PipelineExecutorGL* pipeline) {
            pipelineExecutor = pipeline;
        }

        bool ApplyVBB(
            uint32_t vertexBufferCount,
            const VertexBufferBinding* vertexBuffers,
            OpenGL::ResourceGL& resourceGL,
            const OpenGL::PipelineGL* pipeline,
            std::string& errorMsg);

        DrawResult BindRenderTarget(Handles::RenderTargetHandle renderTargetHandle,
            BeginRenderPassParams params, OpenGL::ResourceGL&
            resourceGL);

        DrawResult BindTexture(Handles::TextureHandle textureTargetHandle,
            BeginRenderPassParams params,
            OpenGL::ResourceGL& resourceGL);

        DrawResult EndDrawRenderTarget(Handles::RenderTargetHandle renderTargetHandle,
            EndRenderPassParams params, OpenGL::ResourceGL&
            resourceGL);

        DrawResult EndDrawTexture(Handles::TextureHandle textureTargetHandle,
            EndRenderPassParams params,
            OpenGL::ResourceGL& resourceGL);


    public:
        DrawExecuteResult ProcessDrawArraysImpl(RHI::Base::BaseDevice& device, const DrawArraysParams& p);
        DrawExecuteResult ProcessDrawElementsImpl(RHI::Base::BaseDevice& device, const DrawElementsParams& p);

        DrawExecuteResult ProcessBeginRenderPassImpl(RHI::Base::BaseDevice& device, const BeginRenderPassParams& p);
        DrawExecuteResult ProcessEndRenderPassImpl(RHI::Base::BaseDevice& device, const EndRenderPassParams& p);

        DrawExecuteResult ProcessSetViewportImpl(RHI::Base::BaseDevice& device, const SetViewportParams& p);
        DrawExecuteResult ProcessSetScissorImpl(RHI::Base::BaseDevice& device, const SetScissorParams& p);
        DrawExecuteResult ProcessSetBlendConstantsImpl(RHI::Base::BaseDevice& device, const SetBlendConstantsParams& p);
        DrawExecuteResult ProcessBindUniformBufferImpl(RHI::Base::BaseDevice& device, const BindUniformBufferParams& p);
        DrawExecuteResult ProcessBindStorageBufferImpl(RHI::Base::BaseDevice& device, const BindStorageBufferParams& p);

        bool IsValid() override;
    };
}