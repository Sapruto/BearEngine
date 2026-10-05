#pragma once

#include "RHI/Base/Executors/BufferExecutor.h"
#include "RHI/Base/Models/BufferExecutorParams.h"
#include "RHI/Base/Models/BufferExecutorResult.h"

#include "glad/glad.h"

#include <string>

namespace RHI::Executors::BufferExecute {
    class BufferExecutorGL final : public BufferExecutor<BufferExecutorGL> {
    private:
        bool CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode);
        void BindFBO(unsigned int fboId, BindTarget bindTarget);
        std::string GetFBOStatusString(GLenum status);
        GLenum GetGLAttachment(AttachmentType attachment);
        GLenum GetGLBlitMask(BlitMask mask);
        GLenum GetGLFilter(FilterMode filter);
        
        GLenum GetGLUsage(BufferUsage usage);
        GLenum GetGLIndexType(IndexType type);
        GLenum GetGLAttributeType(AttributeType type);
        GLsizei GetAttributeSize(AttributeType type);

        size_t GetBufferSize(GLenum target, GLuint bufferID);
        const void* ExtractDataFromVariant(const IBOUpdateParams& params, size_t& elementSize);

    public:
        BufferExecuteResult ProcessFBOCreateImpl(RHI::Base::BaseDevice& device, const FBOCreateParams& params);
        BufferExecuteResult ProcessFBOAttachImpl(RHI::Base::BaseDevice& device, const FBOAttachTextureParams& params);
        BufferExecuteResult ProcessFBOChangeImpl(RHI::Base::BaseDevice& device, const FBOChangeParams& params);
        BufferExecuteResult ProcessFBOBlitImpl(RHI::Base::BaseDevice& device, const FBOBlitParams& params);
        
        BufferExecuteResult ProcessVBOCreateImpl(RHI::Base::BaseDevice& device, const VBOCreateParams& params);
        BufferExecuteResult ProcessVBOUpdateImpl(RHI::Base::BaseDevice& device, const VBOUpdateParams& params);
        BufferExecuteResult ProcessVBOMapImpl(RHI::Base::BaseDevice& device, const VBOMapParams& params);
        BufferExecuteResult ProcessVBOUnmapImpl(RHI::Base::BaseDevice& device, const VBOUnmapParams& params);
        
        BufferExecuteResult ProcessIBOCreateImpl(RHI::Base::BaseDevice& device, const IBOCreateParams& params);
        BufferExecuteResult ProcessIBOUpdateImpl(RHI::Base::BaseDevice& device, const IBOUpdateParams& params);
        
        BufferExecuteResult ProcessVAOCreateImpl(RHI::Base::BaseDevice& device, const VAOCreateParams& params);
        BufferExecuteResult ProcessVAOSetAttributeImpl(RHI::Base::BaseDevice& device, const VAOSetAttributeParams& params);
        
        BufferExecuteResult ProcessBufferDestroyImpl(RHI::Base::BaseDevice& device, const BufferDestroyParams& params);

        bool IsValid() override;
    };
}