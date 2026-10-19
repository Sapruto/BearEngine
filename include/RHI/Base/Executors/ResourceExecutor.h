#pragma once

#include <concepts>

#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::ResourceExecutor {
    template<typename T>
    concept HasResourceExecutorImpl = requires(
        T* t, RHI::Base::BaseDevice& device,
        const CreateBufferParams& createBuffer,
        const UpdateBufferParams& updateBuffer,
        const CopyBufferParams& copyBuffer,
        const DestroyBufferParams& destroyBuffer,
        const CreateTextureParams& createTexture,
        const UpdateTextureParams& updateTexture,
        const CopyTextureParams& copyTexture,
        const DestroyTextureParams& destroyTexture,
        const CreateSamplerParams& createSampler,
        const DestroySamplerParams& destroySampler,
        const CreateShaderParams& createShader,
        const DestroyShaderParams& destroyShader,
        const CreateRenderTargetParams& createRenderTarget,
        const DestroyRenderTargetParams& destroyRenderTarget) {
        { t->ProcessCreateBufferImpl(device, createBuffer) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessUpdateBufferImpl(device, updateBuffer) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessCopyBufferImpl(device, copyBuffer) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessDestroyBufferImpl(device, destroyBuffer) } -> std::same_as<ResourceExecuteResult>;

        { t->ProcessCreateTextureImpl(device, createTexture) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessUpdateTextureImpl(device, updateTexture) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessCopyTextureImpl(device, copyTexture) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessDestroyTextureImpl(device, destroyTexture) } -> std::same_as<ResourceExecuteResult>;

        { t->ProcessCreateSamplerImpl(device, createSampler) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessDestroySamplerImpl(device, destroySampler) } -> std::same_as<ResourceExecuteResult>;

        { t->ProcessCreateShaderImpl(device, createShader) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessDestroyShaderImpl(device, destroyShader) } -> std::same_as<ResourceExecuteResult>;

        { t->ProcessCreateRenderTargetImpl(device, createRenderTarget) } -> std::same_as<ResourceExecuteResult>;
        { t->ProcessDestroyRenderTargetImpl(device, destroyRenderTarget) } -> std::same_as<ResourceExecuteResult>;

        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename ResourceExecutorImpl>
    class ResourceExecutor : public BaseExecutorRHI<ResourceExecutorImpl, ResourceParams, ResourceExecuteResult> {
    public:
        ResourceExecutor() {
            static_assert(HasResourceExecutorImpl<ResourceExecutorImpl>,
                        "ResourceExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::Resources));
            this->type = ExecutorTypeRHI::ResourceExecutor;
        }

        ResourceExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const ResourceParams& param) {
            auto* self = static_cast<ResourceExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> ResourceExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, CreateBufferParams>) {
                    return self->ProcessCreateBufferImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, UpdateBufferParams>) {
                    return self->ProcessUpdateBufferImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CopyBufferParams>) {
                    return self->ProcessCopyBufferImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroyBufferParams>) {
                    return self->ProcessDestroyBufferImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CreateTextureParams>) {
                    return self->ProcessCreateTextureImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, UpdateTextureParams>) {
                    return self->ProcessUpdateTextureImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CopyTextureParams>) {
                    return self->ProcessCopyTextureImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroyTextureParams>) {
                    return self->ProcessDestroyTextureImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CreateSamplerParams>) {
                    return self->ProcessCreateSamplerImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroySamplerParams>) {
                    return self->ProcessDestroySamplerImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CreateShaderParams>) {
                    return self->ProcessCreateShaderImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroyShaderParams>) {
                    return self->ProcessDestroyShaderImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, CreateRenderTargetParams>) {
                    return self->ProcessCreateRenderTargetImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroyRenderTargetParams>) {
                    return self->ProcessDestroyRenderTargetImpl(device, p);
                }
                else {
                    ResourceExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown resource param type";
                    return result;
                }
            }, param);
        }

        ResourceExecuteResult ProcessCreateBuffer(RHI::Base::BaseDevice& device, const CreateBufferParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCreateBufferImpl(device, p);
        }

        ResourceExecuteResult ProcessUpdateBuffer(RHI::Base::BaseDevice& device, const UpdateBufferParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessUpdateBufferImpl(device, p);
        }

        ResourceExecuteResult ProcessCopyBuffer(RHI::Base::BaseDevice& device, const CopyBufferParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCopyBufferImpl(device, p);
        }

        ResourceExecuteResult ProcessDestroyBuffer(RHI::Base::BaseDevice& device, const DestroyBufferParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessDestroyBufferImpl(device, p);
        }

        ResourceExecuteResult ProcessCreateTexture(RHI::Base::BaseDevice& device, const CreateTextureParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCreateTextureImpl(device, p);
        }

        ResourceExecuteResult ProcessUpdateTexture(RHI::Base::BaseDevice& device, const UpdateTextureParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessUpdateTextureImpl(device, p);
        }

        ResourceExecuteResult ProcessCopyTexture(RHI::Base::BaseDevice& device, const CopyTextureParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCopyTextureImpl(device, p);
        }

        ResourceExecuteResult ProcessDestroyTexture(RHI::Base::BaseDevice& device, const DestroyTextureParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessDestroyTextureImpl(device, p);
        }

        ResourceExecuteResult ProcessCreateSampler(RHI::Base::BaseDevice& device, const CreateSamplerParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCreateSamplerImpl(device, p);
        }

        ResourceExecuteResult ProcessDestroySampler(RHI::Base::BaseDevice& device, const DestroySamplerParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessDestroySamplerImpl(device, p);
        }

        ResourceExecuteResult ProcessCreateShader(RHI::Base::BaseDevice& device, const CreateShaderParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCreateShaderImpl(device, p);
        }

        ResourceExecuteResult ProcessDestroyShader(RHI::Base::BaseDevice& device, const DestroyShaderParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessDestroyShaderImpl(device, p);
        }

        ResourceExecuteResult ProcessCreateRenderTarget(RHI::Base::BaseDevice& device, const CreateRenderTargetParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessCreateRenderTargetImpl(device, p);
        }

        ResourceExecuteResult ProcessDestroyRenderTarget(RHI::Base::BaseDevice& device, const DestroyRenderTargetParams& p) {
            return static_cast<ResourceExecutorImpl*>(this)->ProcessDestroyRenderTargetImpl(device, p);
        }
    };
}