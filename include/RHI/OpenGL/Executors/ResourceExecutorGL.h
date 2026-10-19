#pragma once

#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"
#include "RHI/Base/Executors/ResourceExecutor.h"

namespace RHI::Executors::ResourceExecutor {
    class ResourceExecutorGL final : public ResourceExecutor<ResourceExecutorGL> {
    public:
        ResourceExecuteResult ProcessCreateBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateBufferParams&);
        ResourceExecuteResult ProcessUpdateBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::UpdateBufferParams&);
        ResourceExecuteResult ProcessCopyBufferImpl  (RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CopyBufferParams&);
        ResourceExecuteResult ProcessDestroyBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyBufferParams&);

        ResourceExecuteResult ProcessCreateTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateTextureParams&);
        ResourceExecuteResult ProcessUpdateTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::UpdateTextureParams&);
        ResourceExecuteResult ProcessCopyTextureImpl  (RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CopyTextureParams&);
        ResourceExecuteResult ProcessDestroyTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyTextureParams&);

        ResourceExecuteResult ProcessCreateSamplerImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateSamplerParams&);
        ResourceExecuteResult ProcessDestroySamplerImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroySamplerParams&);

        ResourceExecuteResult ProcessCreateShaderImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateShaderParams&);
        ResourceExecuteResult ProcessDestroyShaderImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyShaderParams&);

        ResourceExecuteResult ProcessCreateRenderTargetImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateRenderTargetParams&);
        ResourceExecuteResult ProcessDestroyRenderTargetImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyRenderTargetParams&);

        bool IsValid() override;
    };
}