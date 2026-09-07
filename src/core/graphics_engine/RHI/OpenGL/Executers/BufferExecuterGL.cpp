#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace BufferExecute {
    bool BufferExecuterGL::CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            errorMsg = std::string("Error ") + op + ": " + std::to_string(err);
            errorCode = err;
            return false;
        }
        return true;
    }

    void BufferExecuterGL::BindFBO(unsigned int fboId, BindTarget bindTarget) {
        switch (bindTarget) {
            case BindTarget::ReadOnly:
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
                break;
            case BindTarget::WriteOnly:
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);
                break;
            case BindTarget::ReadWrite:
                glBindFramebuffer(GL_FRAMEBUFFER, fboId);
                break;
            case BindTarget::Undefined:
                break;
        }
    }

    std::string BufferExecuterGL::GetFBOStatusString(GLenum status) {
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE: return "Complete";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "Incomplete attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "Missing attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "Incomplete draw buffer";
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "Incomplete read buffer";
            case GL_FRAMEBUFFER_UNSUPPORTED: return "Unsupported";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "Incomplete multisample";
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "Incomplete layer targets";
            default: return "Unknown error: " + std::to_string(status);
        }
    }

    GLenum BufferExecuterGL::GetGLAttachment(AttachmentType attachment) {
        switch (attachment) {
            case AttachmentType::Color: return GL_COLOR_ATTACHMENT0;
            case AttachmentType::Depth: return GL_DEPTH_ATTACHMENT;
            case AttachmentType::Stencil: return GL_STENCIL_ATTACHMENT;
            case AttachmentType::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
            default: return GL_COLOR_ATTACHMENT0;
        }
    }

    GLenum BufferExecuterGL::GetGLBlitMask(BlitMask mask) {
        switch (mask) {
            case BlitMask::Color: return GL_COLOR_BUFFER_BIT;
            case BlitMask::Depth: return GL_DEPTH_BUFFER_BIT;
            case BlitMask::Stencil: return GL_STENCIL_BUFFER_BIT;
            case BlitMask::DepthStencil: return GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
            default: return GL_COLOR_BUFFER_BIT;
        }
    }

    GLenum BufferExecuterGL::GetGLFilter(FilterMode filter) {
        return filter == FilterMode::Linear ? GL_LINEAR : GL_NEAREST;
    }

    GLenum BufferExecuterGL::GetGLUsage(BufferUsage usage) {
        switch (usage) {
            case BufferUsage::Static: return GL_STATIC_DRAW;
            case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
            case BufferUsage::Stream: return GL_STREAM_DRAW;
            case BufferUsage::StaticRead: return GL_STATIC_READ;
            case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
            case BufferUsage::StreamRead: return GL_STREAM_READ;
            default: return GL_STATIC_DRAW;
        }
    }

    GLenum BufferExecuterGL::GetGLIndexType(IndexType type) {
        switch (type) {
            case IndexType::UByte: return GL_UNSIGNED_BYTE;
            case IndexType::UShort: return GL_UNSIGNED_SHORT;
            case IndexType::UInt: return GL_UNSIGNED_INT;
            default: return GL_UNSIGNED_INT;
        }
    }

    GLenum BufferExecuterGL::GetGLAttributeType(AttributeType type) {
        if (type >= AttributeType::Float && type <= AttributeType::Float4) return GL_FLOAT;
        if (type >= AttributeType::Int && type <= AttributeType::Int4) return GL_INT;
        if (type >= AttributeType::UInt && type <= AttributeType::UInt4) return GL_UNSIGNED_INT;
        if (type >= AttributeType::Matrix2x2 && type <= AttributeType::Matrix4x4) return GL_FLOAT;
        return GL_FLOAT;
    }

    GLsizei BufferExecuterGL::GetAttributeSize(AttributeType type) {
        switch (type) {
            case AttributeType::Float: return 1;
            case AttributeType::Float2: return 2;
            case AttributeType::Float3: return 3;
            case AttributeType::Float4: return 4;
            case AttributeType::Int: return 1;
            case AttributeType::Int2: return 2;
            case AttributeType::Int3: return 3;
            case AttributeType::Int4: return 4;
            case AttributeType::UInt: return 1;
            case AttributeType::UInt2: return 2;
            case AttributeType::UInt3: return 3;
            case AttributeType::UInt4: return 4;
            case AttributeType::Matrix2x2: return 4;
            case AttributeType::Matrix3x3: return 9;
            case AttributeType::Matrix4x4: return 16;
            default: return 1;
        }
    }

    size_t BufferExecuterGL::GetBufferSize(GLenum target, GLuint bufferID) {
        glBindBuffer(target, bufferID);
        GLint size = 0;
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
        glBindBuffer(target, 0);
        return static_cast<size_t>(size);
    }

    const void* BufferExecuterGL::ExtractDataFromVariant(const IBOUpdateParams& params, size_t& elementSize) {
        const void* data = nullptr;
        std::visit([&](const auto& vec) {
            using T = typename std::decay_t<decltype(vec)>::value_type;
            data = vec.data();
            elementSize = sizeof(T);
        }, params.data);
        
        size_t expectedSize = params.indexType == IndexType::UByte ? 1 :
                             params.indexType == IndexType::UShort ? 2 : 4;
        if (elementSize != expectedSize) {
            return nullptr;
        }
        return data;
    }


    BufferExecuteResult BufferExecuterGL::ProcessFBOCreateImpl(BaseDevice& device, const FBOCreateParams& params) {
        GLuint fboId = 0;
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        glGenFramebuffers(1, &fboId);
        success = CheckGLError("glGenFramebuffers", errorMsg, errorCode);
        
        if (success && params.isBind && fboId != 0) {
            BindFBO(fboId, params.bindTarget);
            success = CheckGLError("glBindFramebuffer", errorMsg, errorCode);
        }
        
        FBOCreateResult result;
        result.fboID = fboId;
        result.isGenerated = success;
        result.isBound = success;
        result.errorCode = errorCode;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessFBOAttachImpl(BaseDevice& device, const FBOAttachTextureParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.fboID == 0) {
            FBOAttachTextureResult result;
            result.fboID = params.fboID;
            result.textureID = params.textureID;
            result.isAttached = false;
            result.attachmentPoint = static_cast<int>(params.attachment);
            result.errorMessage = "Invalid FBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        BindFBO(params.fboID, BindTarget::ReadWrite);
        success = CheckGLError("glBindFramebuffer", errorMsg, errorCode);
        
        if (success) {
            GLenum attachment = GetGLAttachment(params.attachment);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, params.textureID, params.mipLevel);
            success = CheckGLError("glFramebufferTexture2D", errorMsg, errorCode);
        }
        
        if (success) {
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                errorMsg = "FBO incomplete: " + GetFBOStatusString(status);
                errorCode = status;
                success = false;
            }
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        FBOAttachTextureResult result;
        result.fboID = params.fboID;
        result.textureID = params.textureID;
        result.isAttached = success;
        result.attachmentPoint = static_cast<int>(params.attachment);
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessFBOChangeImpl(BaseDevice& device, const FBOChangeParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.fboID == 0) {
            FBOChangeResult result;
            result.fboID = params.fboID;
            result.isRebound = false;
            result.statusValid = false;
            result.isComplete = false;
            result.statusMessage = "Invalid FBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        if (params.isRebind && params.newBindTarget != BindTarget::Undefined) {
            BindFBO(params.fboID, params.newBindTarget);
            success = CheckGLError("glBindFramebuffer", errorMsg, errorCode);
        }
        
        if (success && params.isCheckStatus) {
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                errorMsg = "FBO incomplete: " + GetFBOStatusString(status);
                errorCode = status;
                success = false;
            }
        }
        
        FBOChangeResult result;
        result.fboID = params.fboID;
        result.isRebound = success;
        result.statusValid = success;
        result.isComplete = success;
        result.statusMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessFBOBlitImpl(BaseDevice& device, const FBOBlitParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        size_t bytesTransferred = 0;
        
        if (params.sourceFBO == 0 || params.destFBO == 0) {
            FBOBlitResult result;
            result.sourceFBO = params.sourceFBO;
            result.destFBO = params.destFBO;
            result.isBlitted = false;
            result.bytesTransferred = 0;
            result.transferTimeMs = 0.0f;
            result.errorMessage = "Invalid FBO ID";
            return BufferExecuteResult(result);
        }
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, params.sourceFBO);
        success = CheckGLError("binding source FBO", errorMsg, errorCode);
        
        if (success) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, params.destFBO);
            success = CheckGLError("binding dest FBO", errorMsg, errorCode);
        }
        
        if (success) {
            bytesTransferred = params.sourceRect.width * params.sourceRect.height * 4;
            glBlitFramebuffer(
                params.sourceRect.x, params.sourceRect.y,
                params.sourceRect.x + params.sourceRect.width, params.sourceRect.y + params.sourceRect.height,
                params.destRect.x, params.destRect.y,
                params.destRect.x + params.destRect.width, params.destRect.y + params.destRect.height,
                GetGLBlitMask(params.mask), GetGLFilter(params.filter)
            );
            success = CheckGLError("glBlitFramebuffer", errorMsg, errorCode);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        FBOBlitResult result;
        result.sourceFBO = params.sourceFBO;
        result.destFBO = params.destFBO;
        result.isBlitted = success;
        result.bytesTransferred = bytesTransferred;
        result.transferTimeMs = 0.0f;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }


    BufferExecuteResult BufferExecuterGL::ProcessVBOCreateImpl(BaseDevice& device, const VBOCreateParams& params) {
        GLuint vboId = 0;
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        size_t allocatedSize = 0;
        
        if (params.isGen) {
            glGenBuffers(1, &vboId);
            success = CheckGLError("glGenBuffers", errorMsg, errorCode);
        }
        
        if (success && params.isBind && vboId != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        }
        
        if (success && params.data != nullptr && params.size > 0 && vboId != 0) {
            glBufferData(GL_ARRAY_BUFFER, params.size, params.data, GetGLUsage(params.usage));
            success = CheckGLError("glBufferData", errorMsg, errorCode);
            allocatedSize = params.size;
        }
        
        VBOCreateResult result;
        result.vboID = vboId;
        result.allocatedSize = allocatedSize;
        result.isGenerated = success;
        result.isBound = success;
        result.actualUsage = params.usage;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessVBOUpdateImpl(BaseDevice& device, const VBOUpdateParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        size_t bytesUpdated = 0;
        size_t totalSize = 0;
        
        if (params.vboID == 0) {
            VBOUpdateResult result;
            result.vboID = params.vboID;
            result.bytesUpdated = 0;
            result.totalSize = 0;
            result.isFullUpdate = params.isFullUpdate;
            result.success = false;
            result.errorMessage = "Invalid VBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, params.vboID);
        success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        
        if (success) {
            totalSize = GetBufferSize(GL_ARRAY_BUFFER, params.vboID);
            
            if (params.isFullUpdate) {
                glBufferData(GL_ARRAY_BUFFER, params.size, params.data, GL_DYNAMIC_DRAW);
                success = CheckGLError("glBufferData", errorMsg, errorCode);
                bytesUpdated = params.size;
                totalSize = params.size;
            }
            else {
                glBufferSubData(GL_ARRAY_BUFFER, params.offset, params.size, params.data);
                success = CheckGLError("glBufferSubData", errorMsg, errorCode);
                bytesUpdated = params.size;
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        VBOUpdateResult result;
        result.vboID = params.vboID;
        result.bytesUpdated = bytesUpdated;
        result.totalSize = totalSize;
        result.isFullUpdate = params.isFullUpdate;
        result.success = success;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessVBOMapImpl(BaseDevice& device, const VBOMapParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        void* mappedPtr = nullptr;
        size_t mappedSize = 0;
        
        if (params.vboID == 0) {
            VBOMapResult result;
            result.vboID = params.vboID;
            result.mappedPtr = nullptr;
            result.mappedSize = 0;
            result.isMapped = false;
            result.isReadOnly = params.isReadOnly;
            result.isWriteOnly = params.isWriteOnly;
            result.errorMessage = "Invalid VBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, params.vboID);
        success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        
        if (success) {
            mappedSize = GetBufferSize(GL_ARRAY_BUFFER, params.vboID);
            
            GLenum access = params.isReadOnly ? GL_READ_ONLY :
                        params.isWriteOnly ? GL_WRITE_ONLY : GL_READ_WRITE;
            
            if (params.offset == 0 && params.length == 0) {
                mappedPtr = glMapBuffer(GL_ARRAY_BUFFER, access);
            }
            else {
                GLbitfield flags = 0;
                if (params.isReadOnly) flags |= GL_MAP_READ_BIT;
                if (params.isWriteOnly) flags |= GL_MAP_WRITE_BIT;
                if (params.isReadWrite) flags |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
                mappedPtr = glMapBufferRange(GL_ARRAY_BUFFER, params.offset, params.length, flags);
                mappedSize = params.length;
            }
            success = CheckGLError("mapping buffer", errorMsg, errorCode);
            
            if (mappedPtr == nullptr && success) {
                errorMsg = "Map returned null pointer";
                success = false;
            }
        }
        
        VBOMapResult result;
        result.vboID = params.vboID;
        result.mappedPtr = mappedPtr;
        result.mappedSize = mappedSize;
        result.isMapped = success;
        result.isReadOnly = params.isReadOnly;
        result.isWriteOnly = params.isWriteOnly;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessVBOUnmapImpl(BaseDevice& device, const VBOUnmapParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.vboID == 0) {
            VBOUnmapResult result;
            result.vboID = params.vboID;
            result.isUnmapped = false;
            result.dataFlushed = false;
            result.errorMessage = "Invalid VBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, params.vboID);
        success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        
        if (success) {
            GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
            if (!result) {
                errorMsg = "glUnmapBuffer failed";
                success = false;
            }
            if (!CheckGLError("glUnmapBuffer", errorMsg, errorCode)) {
                success = false;
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        VBOUnmapResult result;
        result.vboID = params.vboID;
        result.isUnmapped = success;
        result.dataFlushed = success;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }


    BufferExecuteResult BufferExecuterGL::ProcessIBOCreateImpl(BaseDevice& device, const IBOCreateParams& params) {
        GLuint iboId = 0;
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        size_t indexCount = 0;
        
        if (params.isGen) {
            glGenBuffers(1, &iboId);
            success = CheckGLError("glGenBuffers", errorMsg, errorCode);
        }
        
        if (success && params.isBind && iboId != 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
            success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        }
        
        if (success && params.data != nullptr && params.count > 0 && iboId != 0) {
            size_t elementSize = params.indexType == IndexType::UByte ? 1 :
                                params.indexType == IndexType::UShort ? 2 : 4;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, params.count * elementSize, params.data, GetGLUsage(params.usage));
            success = CheckGLError("glBufferData", errorMsg, errorCode);
            indexCount = params.count;
        }
        
        IBOCreateResult result;
        result.iboID = iboId;
        result.indexCount = indexCount;
        result.indexType = params.indexType;
        result.isGenerated = success;
        result.isBound = success;
        result.actualUsage = params.usage;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessIBOUpdateImpl(BaseDevice& device, const IBOUpdateParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        size_t indicesUpdated = 0;
        size_t totalIndices = 0;
        
        if (params.iboID == 0) {
            IBOUpdateResult result;
            result.iboID = params.iboID;
            result.indicesUpdated = 0;
            result.totalIndices = 0;
            result.isFullUpdate = params.isFullUpdate;
            result.success = false;
            result.errorMessage = "Invalid IBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        if (params.data.valueless_by_exception()) {
            IBOUpdateResult result;
            result.iboID = params.iboID;
            result.indicesUpdated = 0;
            result.totalIndices = 0;
            result.isFullUpdate = params.isFullUpdate;
            result.success = false;
            result.errorMessage = "Invalid data in variant";
            return BufferExecuteResult(result);
        }
        
        if (params.count == 0) {
            IBOUpdateResult result;
            result.iboID = params.iboID;
            result.indicesUpdated = 0;
            result.totalIndices = 0;
            result.isFullUpdate = params.isFullUpdate;
            result.success = false;
            result.errorMessage = "Zero count provided";
            return BufferExecuteResult(result);
        }
        
        size_t elementSize = 0;
        const void* dataPtr = ExtractDataFromVariant(params, elementSize);
        
        if (dataPtr == nullptr) {
            IBOUpdateResult result;
            result.iboID = params.iboID;
            result.indicesUpdated = 0;
            result.totalIndices = 0;
            result.isFullUpdate = params.isFullUpdate;
            result.success = false;
            result.errorMessage = "Data type mismatch with indexType";
            return BufferExecuteResult(result);
        }
        
        size_t dataSize = params.count * elementSize;
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, params.iboID);
        success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        
        if (success) {
            size_t bufferSize = GetBufferSize(GL_ELEMENT_ARRAY_BUFFER, params.iboID);
            totalIndices = bufferSize / elementSize;
            
            if (params.isFullUpdate) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, dataPtr, GL_DYNAMIC_DRAW);
                success = CheckGLError("glBufferData", errorMsg, errorCode);
                indicesUpdated = params.count;
                totalIndices = params.count;
            }
            else {
                if (params.offset + dataSize > bufferSize) {
                    errorMsg = "Buffer overflow: offset + size exceeds buffer size";
                    success = false;
                }
                else {
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, params.offset, dataSize, dataPtr);
                    success = CheckGLError("glBufferSubData", errorMsg, errorCode);
                    indicesUpdated = params.count;
                }
            }
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        IBOUpdateResult result;
        result.iboID = params.iboID;
        result.indicesUpdated = indicesUpdated;
        result.totalIndices = totalIndices;
        result.isFullUpdate = params.isFullUpdate;
        result.success = success;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }


    BufferExecuteResult BufferExecuterGL::ProcessVAOCreateImpl(BaseDevice& device, const VAOCreateParams& params) {
        GLuint vaoId = 0;
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.isGen) {
            glGenVertexArrays(1, &vaoId);
            success = CheckGLError("glGenVertexArrays", errorMsg, errorCode);
        }
        
        if (success && params.isBind && vaoId != 0) {
            glBindVertexArray(vaoId);
            success = CheckGLError("glBindVertexArray", errorMsg, errorCode);
        }
        
        if (success && params.vboID != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, params.vboID);
            success = CheckGLError("binding VBO", errorMsg, errorCode);
        }
        
        if (success && params.iboID != 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, params.iboID);
            success = CheckGLError("binding IBO", errorMsg, errorCode);
        }
        
        VAOCreateResult result;
        result.vaoID = vaoId;
        result.vboID = params.vboID;
        result.iboID = params.iboID;
        result.isGenerated = success;
        result.isBound = success;
        result.isValid = success;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }

    BufferExecuteResult BufferExecuterGL::ProcessVAOSetAttributeImpl(BaseDevice& device, const VAOSetAttributeParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.vaoID == 0) {
            VAOSetAttributeResult result;
            result.vaoID = params.vaoID;
            result.attributeIndex = params.index;
            result.isSet = false;
            result.isEnabled = params.isEnable;
            result.stride = params.stride;
            result.offset = params.offset;
            result.errorMessage = "Invalid VAO ID: 0";
            return BufferExecuteResult(result);
        }
        
        if (params.vboID == 0) {
            VAOSetAttributeResult result;
            result.vaoID = params.vaoID;
            result.attributeIndex = params.index;
            result.isSet = false;
            result.isEnabled = params.isEnable;
            result.stride = params.stride;
            result.offset = params.offset;
            result.errorMessage = "Invalid VBO ID: 0";
            return BufferExecuteResult(result);
        }
        
        glBindVertexArray(params.vaoID);
        success = CheckGLError("glBindVertexArray", errorMsg, errorCode);
        
        if (success) {
            glBindBuffer(GL_ARRAY_BUFFER, params.vboID);
            success = CheckGLError("glBindBuffer", errorMsg, errorCode);
        }
        
        if (success) {
            GLenum attrType = GetGLAttributeType(params.type);
            GLsizei size = GetAttributeSize(params.type);
            
            if (params.isInteger) {
                glVertexAttribIPointer(params.index, size, attrType, params.stride, (const void*)params.offset);
            }
            else {
                glVertexAttribPointer(params.index, size, attrType, params.isNormalized ? GL_TRUE : GL_FALSE,
                                    params.stride, (const void*)params.offset);
            }
            success = CheckGLError("glVertexAttribPointer", errorMsg, errorCode);
        }
        
        if (success) {
            if (params.isEnable) {
                glEnableVertexAttribArray(params.index);
            }
            else {
                glDisableVertexAttribArray(params.index);
            }
            success = CheckGLError("enabling/disabling attrib", errorMsg, errorCode);
        }
        
        if (success && params.divisor > 0) {
            glVertexAttribDivisor(params.index, params.divisor);
            success = CheckGLError("glVertexAttribDivisor", errorMsg, errorCode);
        }
        
        VAOSetAttributeResult result;
        result.vaoID = params.vaoID;
        result.attributeIndex = params.index;
        result.isSet = success;
        result.isEnabled = params.isEnable;
        result.stride = params.stride;
        result.offset = params.offset;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }


    BufferExecuteResult BufferExecuterGL::ProcessBufferDestroyImpl(BaseDevice& device, const BufferDestroyParams& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        
        if (params.bufferId == 0) {
            BufferDestroyResult result;
            result.bufferId = params.bufferId;
            result.isDestroyed = false;
            result.resourcesFreed = false;
            result.errorMessage = "Invalid buffer ID: 0";
            return BufferExecuteResult(result);
        }
        
        switch (params.bufferType) {
            case BufferType::Framebuffer:
                glDeleteFramebuffers(1, &params.bufferId);
                break;
            case BufferType::VAO:
                glDeleteVertexArrays(1, &params.bufferId);
                break;
            case BufferType::VBO:
            case BufferType::IBO:
                glDeleteBuffers(1, &params.bufferId);
                break;
        }
        success = CheckGLError("glDeleteBuffer", errorMsg, errorCode);

        BufferDestroyResult result;
        result.bufferId = params.bufferId;
        result.isDestroyed = success;
        result.resourcesFreed = success;
        result.errorMessage = errorMsg;
        return BufferExecuteResult(result);
    }


    bool BufferExecuterGL::IsValid() {
        return glfwGetCurrentContext() != nullptr;
    }
}