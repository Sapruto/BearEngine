#pragma once

#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"
#include "RHI/Base/Executors/ResourceExecutor.h"

namespace RHI::Executors::ResourceExecutor {
    class ResourceExecutorGL final : public ResourceExecutor<ResourceExecutorGL> {
    public:
        CreateBufferResult ProcessCreateBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateBufferParams&);
        UpdateBufferResult ProcessUpdateBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::UpdateBufferParams&);
        CopyBufferResult   ProcessCopyBufferImpl  (RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CopyBufferParams&);
        DestroyBufferResult ProcessDestroyBufferImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyBufferParams&);

        CreateTextureResult ProcessCreateTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateTextureParams&);
        UpdateTextureResult ProcessUpdateTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::UpdateTextureParams&);
        CopyTextureResult   ProcessCopyTextureImpl  (RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CopyTextureParams&);
        DestroyTextureResult ProcessDestroyTextureImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyTextureParams&);

        CreateSamplerResult ProcessCreateSamplerImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateSamplerParams&);
        DestroySamplerResult ProcessDestroySamplerImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroySamplerParams&);

        CreateShaderResult ProcessCreateShaderImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateShaderParams&);
        DestroyShaderResult ProcessDestroyShaderImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyShaderParams&);

        CreateRenderTargetResult ProcessCreateRenderTargetImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::CreateRenderTargetParams&);
        DestroyRenderTargetResult ProcessDestroyRenderTargetImpl(RHI::Base::BaseDevice&, const RHI::Executors::ResourceExecutor::DestroyRenderTargetParams&);

        bool IsValid() override;
    };
}