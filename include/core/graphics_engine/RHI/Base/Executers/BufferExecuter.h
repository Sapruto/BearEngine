#pragma once

#include <concepts>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"

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
    requires(HasBufferExecuterImpl<BufferExecuterImpl>)
    class BufferExecuter : public BaseExecuterRHI<BufferExecuterImpl, BufferParams, BufferExecuteResult> {
    private:
        BufferExecuterImpl* impl;

    protected:
        BufferExecuteResult ProcessFBOCreate(BaseDevice& device, const FBOCreateParams& params) {
            return impl->ProcessFBOCreateImpl(device, params);
        }
        
        BufferExecuteResult ProcessFBOAttach(BaseDevice& device, const FBOAttachTextureParams& params) {
            return impl->ProcessFBOAttachImpl(device, params);
        }
        
        BufferExecuteResult ProcessFBOChange(BaseDevice& device, const FBOChangeParams& params) {
            return impl->ProcessFBOChangeImpl(device, params);
        }
        
        BufferExecuteResult ProcessFBOBlit(BaseDevice& device, const FBOBlitParams& params) {
            return impl->ProcessFBOBlitImpl(device, params);
        }
        
        BufferExecuteResult ProcessVBOCreate(BaseDevice& device, const VBOCreateParams& params) {
            return impl->ProcessVBOCreateImpl(device, params);
        }
        
        BufferExecuteResult ProcessVBOUpdate(BaseDevice& device, const VBOUpdateParams& params) {
            return impl->ProcessVBOUpdateImpl(device, params);
        }
        
        BufferExecuteResult ProcessVBOMap(BaseDevice& device, const VBOMapParams& params) {
            return impl->ProcessVBOMapImpl(device, params);
        }
        
        BufferExecuteResult ProcessVBOUnmap(BaseDevice& device, const VBOUnmapParams& params) {
            return impl->ProcessVBOUnmapImpl(device, params);
        }
        
        BufferExecuteResult ProcessIBOCreate(BaseDevice& device, const IBOCreateParams& params) {
            return impl->ProcessIBOCreateImpl(device, params);
        }
        
        BufferExecuteResult ProcessIBOUpdate(BaseDevice& device, const IBOUpdateParams& params) {
            return impl->ProcessIBOUpdateImpl(device, params);
        }
        
        BufferExecuteResult ProcessVAOCreate(BaseDevice& device, const VAOCreateParams& params) {
            return impl->ProcessVAOCreateImpl(device, params);
        }
        
        BufferExecuteResult ProcessVAOSetAttribute(BaseDevice& device, const VAOSetAttributeParams& params) {
            return impl->ProcessVAOSetAttributeImpl(device, params);
        }
        
        BufferExecuteResult ProcessBufferDestroy(BaseDevice& device, const BufferDestroyParams& params) {
            return impl->ProcessBufferDestroyImpl(device, params);
        }
        
        BufferExecuteResult ProcessCurrentParam(BaseDevice& device, const BufferParams& param) {
            ParamType type = std::visit([](const auto& p) -> ParamType {
                return p.GetType();
            }, param);
            
            switch (type) {
                case ParamType::FBO_CREATE:
                    return ProcessFBOCreate(device, std::get<FBOCreateParams>(param));
                    
                case ParamType::FBO_ATTACH_TEXTURE:
                    return ProcessFBOAttach(device, std::get<FBOAttachTextureParams>(param));
                    
                case ParamType::FBO_CHANGE:
                    return ProcessFBOChange(device, std::get<FBOChangeParams>(param));
                    
                case ParamType::FBO_BLIT:
                    return ProcessFBOBlit(device, std::get<FBOBlitParams>(param));
                    
                case ParamType::VBO_CREATE:
                    return ProcessVBOCreate(device, std::get<VBOCreateParams>(param));
                    
                case ParamType::VBO_UPDATE:
                    return ProcessVBOUpdate(device, std::get<VBOUpdateParams>(param));
                    
                case ParamType::VBO_MAP:
                    return ProcessVBOMap(device, std::get<VBOMapParams>(param));
                    
                case ParamType::VBO_UNMAP:
                    return ProcessVBOUnmap(device, std::get<VBOUnmapParams>(param));
                    
                case ParamType::IBO_CREATE:
                    return ProcessIBOCreate(device, std::get<IBOCreateParams>(param));
                    
                case ParamType::IBO_UPDATE:
                    return ProcessIBOUpdate(device, std::get<IBOUpdateParams>(param));
                    
                case ParamType::VAO_CREATE:
                    return ProcessVAOCreate(device, std::get<VAOCreateParams>(param));
                    
                case ParamType::VAO_SET_ATTRIBUTE:
                    return ProcessVAOSetAttribute(device, std::get<VAOSetAttributeParams>(param));
                    
                case ParamType::BUFFER_DESTROY:
                    return ProcessBufferDestroy(device, std::get<BufferDestroyParams>(param));
                    
                default:
                    BufferExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown parameter type";
                    return result;
            }
        }
        
    public:
        BufferExecuter() : impl(static_cast<BufferExecuterImpl*>(this)) {
            this->type = ExecuterTypeRHI::BufferExecuter;
        }
        
        void ProcessParamsImpl(BaseDevice& device) {
            if (!impl->IsValid()) return;

            std::vector<BufferExecuteResult> results;
            results.reserve(this->params.size());
            
            for (size_t i = 0; i < this->params.size(); ++i) {
                const auto& param = this->params[i];
                
                BufferExecuteResult result = ProcessCurrentParam(device, param);
                result.paramID = static_cast<int>(i);
                
                results.push_back(std::move(result));
            }
            
            this->NotifySubscribers(results);
            this->ClearParams();
        }
    };
}