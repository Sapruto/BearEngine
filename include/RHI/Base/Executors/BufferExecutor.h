#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"
#include "RHI/Base/Models/BufferExecutorParams.h"
#include "RHI/Base/Models/BufferExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::BufferExecute {
    template<typename T>
    concept HasBufferExecutorImpl = requires(T* t, RHI::Base::BaseDevice& device,
                                             const FBOCreateParams& fboCreate,
                                             const FBOAttachTextureParams& fboAttach,
                                             const FBOChangeParams& fboChange,
                                             const FBOBlitParams& fboBlit,
                                             const VBOCreateParams& vboCreate,
                                             const VBOUpdateParams& vboUpdate,
                                             const VBOMapParams& vboMap,
                                             const VBOUnmapParams& vboUnmap,
                                             const IBOCreateParams& iboCreate,
                                             const IBOUpdateParams& iboUpdate,
                                             const VAOCreateParams& vaoCreate,
                                             const VAOSetAttributeParams& vaoSetAttr,
                                             const BufferDestroyParams& bufferDestroy) {
        { t->ProcessFBOCreateImpl(device, fboCreate) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessFBOAttachImpl(device, fboAttach) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessFBOChangeImpl(device, fboChange) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessFBOBlitImpl(device, fboBlit) } -> std::same_as<BufferExecuteResult>;

        { t->ProcessVBOCreateImpl(device, vboCreate) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessVBOUpdateImpl(device, vboUpdate) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessVBOMapImpl(device, vboMap) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessVBOUnmapImpl(device, vboUnmap) } -> std::same_as<BufferExecuteResult>;

        { t->ProcessIBOCreateImpl(device, iboCreate) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessIBOUpdateImpl(device, iboUpdate) } -> std::same_as<BufferExecuteResult>;

        { t->ProcessVAOCreateImpl(device, vaoCreate) } -> std::same_as<BufferExecuteResult>;
        { t->ProcessVAOSetAttributeImpl(device, vaoSetAttr) } -> std::same_as<BufferExecuteResult>;

        { t->ProcessBufferDestroyImpl(device, bufferDestroy) } -> std::same_as<BufferExecuteResult>;

        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename BufferExecutorImpl>
    class BufferExecutor : public BaseExecutorRHI<BufferExecutorImpl, BufferParams, BufferExecuteResult> {
        using Base = BaseExecutorRHI<BufferExecutorImpl, BufferParams, BufferExecuteResult>;

    public:
        BufferExecutor() {
            static_assert(HasBufferExecutorImpl<BufferExecutorImpl>,
                        "BufferExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::Buffer));
            this->type = ExecutorTypeRHI::BufferExecutor;
        }

        BufferExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const BufferParams& param) {
            auto* self = static_cast<BufferExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> BufferExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, FBOCreateParams>) {
                    return self->ProcessFBOCreateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, FBOAttachTextureParams>) {
                    return self->ProcessFBOAttachImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, FBOChangeParams>) {
                    return self->ProcessFBOChangeImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, FBOBlitParams>) {
                    return self->ProcessFBOBlitImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VBOCreateParams>) {
                    return self->ProcessVBOCreateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VBOUpdateParams>) {
                    return self->ProcessVBOUpdateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VBOMapParams>) {
                    return self->ProcessVBOMapImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VBOUnmapParams>) {
                    return self->ProcessVBOUnmapImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, IBOCreateParams>) {
                    return self->ProcessIBOCreateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, IBOUpdateParams>) {
                    return self->ProcessIBOUpdateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VAOCreateParams>) {
                    return self->ProcessVAOCreateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, VAOSetAttributeParams>) {
                    return self->ProcessVAOSetAttributeImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, BufferDestroyParams>) {
                    return self->ProcessBufferDestroyImpl(device, p);
                }
                else {
                    BufferExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown buffer param type";
                    return result;
                }
            }, param);
        }

        BufferExecuteResult ProcessFBOCreate(RHI::Base::BaseDevice& device, const FBOCreateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessFBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessFBOAttach(RHI::Base::BaseDevice& device, const FBOAttachTextureParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessFBOAttachImpl(device, p);
        }

        BufferExecuteResult ProcessFBOChange(RHI::Base::BaseDevice& device, const FBOChangeParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessFBOChangeImpl(device, p);
        }

        BufferExecuteResult ProcessFBOBlit(RHI::Base::BaseDevice& device, const FBOBlitParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessFBOBlitImpl(device, p);
        }

        BufferExecuteResult ProcessVBOCreate(RHI::Base::BaseDevice& device, const VBOCreateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessVBOUpdate(RHI::Base::BaseDevice& device, const VBOUpdateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVBOUpdateImpl(device, p);
        }

        BufferExecuteResult ProcessVBOMap(RHI::Base::BaseDevice& device, const VBOMapParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVBOMapImpl(device, p);
        }

        BufferExecuteResult ProcessVBOUnmap(RHI::Base::BaseDevice& device, const VBOUnmapParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVBOUnmapImpl(device, p);
        }

        BufferExecuteResult ProcessIBOCreate(RHI::Base::BaseDevice& device, const IBOCreateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessIBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessIBOUpdate(RHI::Base::BaseDevice& device, const IBOUpdateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessIBOUpdateImpl(device, p);
        }

        BufferExecuteResult ProcessVAOCreate(RHI::Base::BaseDevice& device, const VAOCreateParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVAOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessVAOSetAttribute(RHI::Base::BaseDevice& device, const VAOSetAttributeParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessVAOSetAttributeImpl(device, p);
        }

        BufferExecuteResult ProcessBufferDestroy(RHI::Base::BaseDevice& device, const BufferDestroyParams& p) {
            return static_cast<BufferExecutorImpl*>(this)->ProcessBufferDestroyImpl(device, p);
        }
    };
}