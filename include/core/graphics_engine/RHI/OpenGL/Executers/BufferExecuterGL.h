#pragma once

#include "RHI/Base/Executers/BufferExecuter.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"

#include "glad/glad.h"

#include <string>

namespace BufferExecute {
    class BufferExecuterGL : public BufferExecuter<BufferExecuterGL> {
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

    protected:
        BufferExecuteResult ProcessFBOCreateImpl(BaseDevice& device, const FBOCreateParams& params);
        BufferExecuteResult ProcessFBOAttachImpl(BaseDevice& device, const FBOAttachTextureParams& params);
        BufferExecuteResult ProcessFBOChangeImpl(BaseDevice& device, const FBOChangeParams& params);
        BufferExecuteResult ProcessFBOBlitImpl(BaseDevice& device, const FBOBlitParams& params);
        
        BufferExecuteResult ProcessVBOCreateImpl(BaseDevice& device, const VBOCreateParams& params);
        BufferExecuteResult ProcessVBOUpdateImpl(BaseDevice& device, const VBOUpdateParams& params);
        BufferExecuteResult ProcessVBOMapImpl(BaseDevice& device, const VBOMapParams& params);
        BufferExecuteResult ProcessVBOUnmapImpl(BaseDevice& device, const VBOUnmapParams& params);
        
        BufferExecuteResult ProcessIBOCreateImpl(BaseDevice& device, const IBOCreateParams& params);
        BufferExecuteResult ProcessIBOUpdateImpl(BaseDevice& device, const IBOUpdateParams& params);
        
        BufferExecuteResult ProcessVAOCreateImpl(BaseDevice& device, const VAOCreateParams& params);
        BufferExecuteResult ProcessVAOSetAttributeImpl(BaseDevice& device, const VAOSetAttributeParams& params);
        
        BufferExecuteResult ProcessBufferDestroyImpl(BaseDevice& device, const BufferDestroyParams& params);

        bool IsValid();
    };
}