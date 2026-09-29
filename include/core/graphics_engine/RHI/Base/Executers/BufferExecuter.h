#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"
#include "RHI/Base/Executers/BaseExecuterLayers.h"

namespace BufferExecute {
    template<typename T>
    concept HasBufferExecuterImpl = requires(T* t, BaseDevice& device,
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

    template<typename BufferExecuterImpl>
    class BufferExecuter : public BaseExecuterRHI<BufferExecuterImpl, BufferParams, BufferExecuteResult> {
        using Base = BaseExecuterRHI<BufferExecuterImpl, BufferParams, BufferExecuteResult>;

    public:
        BufferExecuter() {
            static_assert(HasBufferExecuterImpl<BufferExecuterImpl>,
                        "BufferExecuterImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecuterLayers::Buffer));
            this->type = ExecuterTypeRHI::BufferExecuter;
        }

        BufferExecuteResult ProcessCurrentParam(BaseDevice& device, const BufferParams& param) {
            auto* self = static_cast<BufferExecuterImpl*>(this);
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

        BufferExecuteResult ProcessFBOCreate(BaseDevice& device, const FBOCreateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessFBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessFBOAttach(BaseDevice& device, const FBOAttachTextureParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessFBOAttachImpl(device, p);
        }

        BufferExecuteResult ProcessFBOChange(BaseDevice& device, const FBOChangeParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessFBOChangeImpl(device, p);
        }

        BufferExecuteResult ProcessFBOBlit(BaseDevice& device, const FBOBlitParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessFBOBlitImpl(device, p);
        }

        BufferExecuteResult ProcessVBOCreate(BaseDevice& device, const VBOCreateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessVBOUpdate(BaseDevice& device, const VBOUpdateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVBOUpdateImpl(device, p);
        }

        BufferExecuteResult ProcessVBOMap(BaseDevice& device, const VBOMapParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVBOMapImpl(device, p);
        }

        BufferExecuteResult ProcessVBOUnmap(BaseDevice& device, const VBOUnmapParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVBOUnmapImpl(device, p);
        }

        BufferExecuteResult ProcessIBOCreate(BaseDevice& device, const IBOCreateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessIBOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessIBOUpdate(BaseDevice& device, const IBOUpdateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessIBOUpdateImpl(device, p);
        }

        BufferExecuteResult ProcessVAOCreate(BaseDevice& device, const VAOCreateParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVAOCreateImpl(device, p);
        }

        BufferExecuteResult ProcessVAOSetAttribute(BaseDevice& device, const VAOSetAttributeParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessVAOSetAttributeImpl(device, p);
        }

        BufferExecuteResult ProcessBufferDestroy(BaseDevice& device, const BufferDestroyParams& p) {
            return static_cast<BufferExecuterImpl*>(this)->ProcessBufferDestroyImpl(device, p);
        }
    };
}