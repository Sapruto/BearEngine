#pragma once

#include "RHI/Base/Executers/BufferExecuter.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"

namespace BufferExecute {
    class BufferExecuterGL : public BufferExecuter<BufferExecuterGL> {
    private:
        VBOCreateResult ProcessVBO(BaseDevice& device, const VBOCreateParams& params);
        IBOCreateResult ProcessIBO(BaseDevice& device, const IBOCreateParams& params);
        VAOCreateResult ProcessVAO(BaseDevice& device, const VAOCreateParams& params);
        VBOUpdateResult ProcessVBOUpdate(BaseDevice& device, const VBOUpdateParams& params);
        IBOUpdateResult ProcessIBOUpdate(BaseDevice& device, const IBOUpdateParams& params);
        VBOMapResult ProcessVBOMap(BaseDevice& device, const VBOMapParams& params);
        VBOUnmapResult ProcessVBOUnmap(BaseDevice& device, const VBOUnmapParams& params);
        FBOAttachTextureResult ProcessFBOAttach(BaseDevice& device, const FBOAttachTextureParams& params);
        FBOChangeResult ProcessFBOChange(BaseDevice& device, const FBOChangeParams& params);
        FBOBlitResult ProcessFBOBlit(BaseDevice& device, const FBOBlitParams& params);
        BufferDestroyResult ProcessBufferDestroy(BaseDevice& device, const BufferDestroyParams& params);
        
    public:
        void ProcessParamsImpl(BaseDevice& device) {
            std::vector<BufferExecuteResult> results;
            results.reserve(params.size());
            
            for (const auto& param : params) {
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    
                    BufferExecuteResult result;
                    result.success = true;
                    
                    if constexpr (std::is_same_v<T, VBOCreateParams>) {
                        result.operationType = BufferExecuteResult::OperationType::VBO_CREATE;
                        result.data = ProcessVBO(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, IBOCreateParams>) {
                        result.operationType = BufferExecuteResult::OperationType::IBO_CREATE;
                        result.data = ProcessIBO(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, VAOCreateParams>) {
                        result.operationType = BufferExecuteResult::OperationType::VAO_CREATE;
                        result.data = ProcessVAO(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, VBOUpdateParams>) {
                        result.operationType = BufferExecuteResult::OperationType::VBO_UPDATE;
                        result.data = ProcessVBOUpdate(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, IBOUpdateParams>) {
                        result.operationType = BufferExecuteResult::OperationType::IBO_UPDATE;
                        result.data = ProcessIBOUpdate(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, VBOMapParams>) {
                        result.operationType = BufferExecuteResult::OperationType::VBO_MAP;
                        result.data = ProcessVBOMap(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, VBOUnmapParams>) {
                        result.operationType = BufferExecuteResult::OperationType::VBO_UNMAP;
                        result.data = ProcessVBOUnmap(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, FBOAttachTextureParams>) {
                        result.operationType = BufferExecuteResult::OperationType::FBO_ATTACH_TEXTURE;
                        result.data = ProcessFBOAttach(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, FBOChangeParams>) {
                        result.operationType = BufferExecuteResult::OperationType::FBO_CHANGE;
                        result.data = ProcessFBOChange(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, FBOBlitParams>) {
                        result.operationType = BufferExecuteResult::OperationType::FBO_BLIT;
                        result.data = ProcessFBOBlit(device, arg);
                    }
                    else if constexpr (std::is_same_v<T, BufferDestroyParams>) {
                        result.operationType = BufferExecuteResult::OperationType::BUFFER_DESTROY;
                        result.data = ProcessBufferDestroy(device, arg);
                    }
                    
                    results.push_back(std::move(result));
                    
                }, param);
            }
            
            this->NotifySubscribers(results);
            this->ClearParams();
        }
    };
}