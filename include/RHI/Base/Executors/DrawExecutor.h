#pragma once

#include <concepts>

#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/Base/Contracts/DrawExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::DrawExecutor {
    template<typename T>
    concept HasDrawExecutorImpl = requires(
        T* t, RHI::Base::BaseDevice& device,
        const DrawArraysParams& drawArrays,
        const DrawElementsParams& drawElements,
        const BeginRenderPassParams& beginRenderPass,
        const EndRenderPassParams& endRenderPass,
        const SetViewportParams& setViewport,
        const SetScissorParams& setScissor,
        const SetBlendConstantsParams& setBlendConstants,
        const BindUniformBufferParams& bindUniformBuffer,
        const BindStorageBufferParams& bindStorageBuffer) {
        { t->ProcessDrawArraysImpl(device, drawArrays) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessDrawElementsImpl(device, drawElements) } -> std::same_as<DrawExecuteResult>;

        { t->ProcessBeginRenderPassImpl(device, beginRenderPass) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessEndRenderPassImpl(device, endRenderPass) } -> std::same_as<DrawExecuteResult>;

        { t->ProcessSetViewportImpl(device, setViewport) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessSetScissorImpl(device, setScissor) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessSetBlendConstantsImpl(device, setBlendConstants) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessBindUniformBufferImpl(device, bindUniformBuffer) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessBindStorageBufferImpl(device, bindStorageBuffer) } -> std::same_as<DrawExecuteResult>;

        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename DrawExecutorImpl>
    class DrawExecutor : public BaseExecutorRHI<DrawExecutorImpl, DrawParams, DrawExecuteResult> {
    public:
        DrawExecutor() {
            static_assert(HasDrawExecutorImpl<DrawExecutorImpl>,
                        "DrawExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::Draw));
            this->type = ExecutorTypeRHI::DrawExecutor;
        }

        DrawExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const DrawParams& param) {
            auto* self = static_cast<DrawExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> DrawExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, DrawArraysParams>) {
                    return self->ProcessDrawArrays(device, p);
                }
                else if constexpr (std::is_same_v<T, DrawElementsParams>) {
                    return self->ProcessDrawElements(device, p);
                }
                else if constexpr (std::is_same_v<T, BeginRenderPassParams>) {
                    return self->ProcessBeginRenderPass(device, p);
                }
                else if constexpr (std::is_same_v<T, EndRenderPassParams>) {
                    return self->ProcessEndRenderPass(device, p);
                }
                else if constexpr (std::is_same_v<T, SetViewportParams>) {
                    return self->ProcessSetViewport(device, p);
                }
                else if constexpr (std::is_same_v<T, SetScissorParams>) {
                    return self->ProcessSetScissor(device, p);
                }
                else if constexpr (std::is_same_v<T, SetBlendConstantsParams>) {
                    return self->ProcessSetBlendConstants(device, p);
                }
                else if constexpr (std::is_same_v<T, BindUniformBufferParams>) {
                    return self->ProcessBindUniformBuffer(device, p);
                }
                else {
                    DrawExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown draw param type";
                    return result;
                }
            }, param);
        }

        DrawExecuteResult ProcessDrawArrays(RHI::Base::BaseDevice& device, const DrawArraysParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessDrawArraysImpl(device, p);
        }

        DrawExecuteResult ProcessDrawElements(RHI::Base::BaseDevice& device, const DrawElementsParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessDrawElementsImpl(device, p);
        }

        DrawExecuteResult ProcessBeginRenderPass(RHI::Base::BaseDevice& device, const BeginRenderPassParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessBeginRenderPassImpl(device, p);
        }

        DrawExecuteResult ProcessEndRenderPass(RHI::Base::BaseDevice& device, const EndRenderPassParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessEndRenderPassImpl(device, p);
        }

        DrawExecuteResult ProcessSetViewport(RHI::Base::BaseDevice& device, const SetViewportParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessSetViewportImpl(device, p);
        }

        DrawExecuteResult ProcessSetScissor(RHI::Base::BaseDevice& device, const SetScissorParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessSetScissorImpl(device, p);
        }

        DrawExecuteResult ProcessSetBlendConstants(RHI::Base::BaseDevice& device, const SetBlendConstantsParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessSetBlendConstantsImpl(device, p);
        }

        DrawExecuteResult ProcessBindUniformBuffer(RHI::Base::BaseDevice& device, const BindUniformBufferParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessBindUniformBufferImpl(device, p);
        }

        DrawExecuteResult ProcessBindStorageBuffer(RHI::Base::BaseDevice& device, const BindStorageBufferParams& p) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessBindStorageBufferImpl(device, p);
        }
    };
}