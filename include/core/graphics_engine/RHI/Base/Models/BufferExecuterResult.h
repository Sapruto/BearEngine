#pragma once

#include <string>
#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"

namespace BufferExecute {
    struct FBOCreateResult {
        unsigned int fboID;
        bool isGenerated;
        bool isBound;
        unsigned int errorCode;
        std::string errorMessage;

        FBOCreateResult() = default;
        FBOCreateResult(unsigned int fboID, bool isGenerated, bool isBound,
                        unsigned int errorCode = 0, const std::string& errorMessage = "")
            : fboID(fboID), isGenerated(isGenerated), isBound(isBound),
              errorCode(errorCode), errorMessage(errorMessage) {}
    };

    struct FBOAttachTextureResult {
        unsigned int fboID;
        unsigned int textureID;
        bool isAttached;
        int attachmentPoint;
        std::string errorMessage;

        FBOAttachTextureResult() = default;
        FBOAttachTextureResult(unsigned int fboID, unsigned int textureID, bool isAttached,
                               int attachmentPoint, const std::string& errorMessage = "")
            : fboID(fboID), textureID(textureID), isAttached(isAttached),
              attachmentPoint(attachmentPoint), errorMessage(errorMessage) {}
    };

    struct FBOChangeResult {
        unsigned int fboID;
        bool isRebound;
        bool statusValid;
        bool isComplete;
        std::string statusMessage;

        FBOChangeResult() = default;
        FBOChangeResult(unsigned int fboID, bool isRebound, bool statusValid,
                        bool isComplete, const std::string& statusMessage = "")
            : fboID(fboID), isRebound(isRebound), statusValid(statusValid),
              isComplete(isComplete), statusMessage(statusMessage) {}
    };

    struct FBOBlitResult {
        unsigned int sourceFBO;
        unsigned int destFBO;
        bool isBlitted;
        size_t bytesTransferred;
        float transferTimeMs;
        std::string errorMessage;

        FBOBlitResult() = default;
        FBOBlitResult(unsigned int sourceFBO, unsigned int destFBO, bool isBlitted,
                      size_t bytesTransferred = 0, float transferTimeMs = 0.0f,
                      const std::string& errorMessage = "")
            : sourceFBO(sourceFBO), destFBO(destFBO), isBlitted(isBlitted),
              bytesTransferred(bytesTransferred), transferTimeMs(transferTimeMs),
              errorMessage(errorMessage) {}
    };

    struct VBOCreateResult {
        unsigned int vboID;
        size_t allocatedSize;
        bool isGenerated;
        bool isBound;
        BufferUsage actualUsage;
        std::string errorMessage;

        VBOCreateResult() = default;
        VBOCreateResult(unsigned int vboID, size_t allocatedSize, bool isGenerated,
                        bool isBound, BufferUsage actualUsage,
                        const std::string& errorMessage = "")
            : vboID(vboID), allocatedSize(allocatedSize), isGenerated(isGenerated),
              isBound(isBound), actualUsage(actualUsage), errorMessage(errorMessage) {}
    };

    struct VBOUpdateResult {
        unsigned int vboID;
        size_t bytesUpdated;
        size_t totalSize;
        bool isFullUpdate;
        bool success;
        std::string errorMessage;

        VBOUpdateResult() = default;
        VBOUpdateResult(unsigned int vboID, size_t bytesUpdated, size_t totalSize,
                        bool isFullUpdate, bool success,
                        const std::string& errorMessage = "")
            : vboID(vboID), bytesUpdated(bytesUpdated), totalSize(totalSize),
              isFullUpdate(isFullUpdate), success(success), errorMessage(errorMessage) {}
    };

    struct VBOMapResult {
        unsigned int vboID;
        void* mappedPtr;
        size_t mappedSize;
        bool isMapped;
        bool isReadOnly;
        bool isWriteOnly;
        std::string errorMessage;

        VBOMapResult() = default;
        VBOMapResult(unsigned int vboID, void* mappedPtr, size_t mappedSize,
                     bool isMapped, bool isReadOnly, bool isWriteOnly,
                     const std::string& errorMessage = "")
            : vboID(vboID), mappedPtr(mappedPtr), mappedSize(mappedSize),
              isMapped(isMapped), isReadOnly(isReadOnly), isWriteOnly(isWriteOnly),
              errorMessage(errorMessage) {}
    };

    struct VBOUnmapResult {
        unsigned int vboID;
        bool isUnmapped;
        bool dataFlushed;
        std::string errorMessage;

        VBOUnmapResult() = default;
        VBOUnmapResult(unsigned int vboID, bool isUnmapped, bool dataFlushed,
                       const std::string& errorMessage = "")
            : vboID(vboID), isUnmapped(isUnmapped), dataFlushed(dataFlushed),
              errorMessage(errorMessage) {}
    };

    struct IBOCreateResult {
        unsigned int iboID;
        size_t indexCount;
        IndexType indexType;
        bool isGenerated;
        bool isBound;
        BufferUsage actualUsage;
        std::string errorMessage;

        IBOCreateResult() = default;
        IBOCreateResult(unsigned int iboID, size_t indexCount, IndexType indexType,
                        bool isGenerated, bool isBound, BufferUsage actualUsage,
                        const std::string& errorMessage = "")
            : iboID(iboID), indexCount(indexCount), indexType(indexType),
              isGenerated(isGenerated), isBound(isBound), actualUsage(actualUsage),
              errorMessage(errorMessage) {}
    };

    struct IBOUpdateResult {
        unsigned int iboID;
        size_t indicesUpdated;
        size_t totalIndices;
        bool isFullUpdate;
        bool success;
        std::string errorMessage;

        IBOUpdateResult() = default;
        IBOUpdateResult(unsigned int iboID, size_t indicesUpdated, size_t totalIndices,
                        bool isFullUpdate, bool success,
                        const std::string& errorMessage = "")
            : iboID(iboID), indicesUpdated(indicesUpdated), totalIndices(totalIndices),
              isFullUpdate(isFullUpdate), success(success), errorMessage(errorMessage) {}
    };

    struct VAOCreateResult {
        unsigned int vaoID;
        unsigned int vboID;
        unsigned int iboID;
        bool isGenerated;
        bool isBound;
        bool isValid;
        std::string errorMessage;

        VAOCreateResult() = default;
        VAOCreateResult(unsigned int vaoID, unsigned int vboID, unsigned int iboID,
                        bool isGenerated, bool isBound, bool isValid,
                        const std::string& errorMessage = "")
            : vaoID(vaoID), vboID(vboID), iboID(iboID),
              isGenerated(isGenerated), isBound(isBound), isValid(isValid),
              errorMessage(errorMessage) {}
    };

    struct VAOSetAttributeResult {
        unsigned int vaoID;
        unsigned int attributeIndex;
        bool isSet;
        bool isEnabled;
        size_t stride;
        size_t offset;
        std::string errorMessage;

        VAOSetAttributeResult() = default;
        VAOSetAttributeResult(unsigned int vaoID, unsigned int attributeIndex,
                              bool isSet, bool isEnabled, size_t stride, size_t offset,
                              const std::string& errorMessage = "")
            : vaoID(vaoID), attributeIndex(attributeIndex), isSet(isSet),
              isEnabled(isEnabled), stride(stride), offset(offset),
              errorMessage(errorMessage) {}
    };

    struct BufferDestroyResult {
        unsigned int bufferId;
        bool isDestroyed;
        bool resourcesFreed;
        std::string errorMessage;

        BufferDestroyResult() = default;
        BufferDestroyResult(unsigned int bufferId, bool isDestroyed, bool resourcesFreed,
                            const std::string& errorMessage = "")
            : bufferId(bufferId), isDestroyed(isDestroyed), resourcesFreed(resourcesFreed),
              errorMessage(errorMessage) {}
    };

    struct BufferExecuteResult : public BaseProcessResult {
        enum class OperationType {
            FBO_CREATE,
            FBO_ATTACH_TEXTURE,
            FBO_CHANGE,
            FBO_BLIT,
            VBO_CREATE,
            VBO_UPDATE,
            VBO_MAP,
            VBO_UNMAP,
            IBO_CREATE,
            IBO_UPDATE,
            VAO_CREATE,
            VAO_SET_ATTRIBUTE,
            BUFFER_DESTROY,
            UNKNOWN
        };

        OperationType operationType{OperationType::UNKNOWN};
        
        std::variant<
                FBOCreateResult,
                FBOAttachTextureResult,
                FBOChangeResult,
                FBOBlitResult,
                VBOCreateResult,
                VBOUpdateResult,
                VBOMapResult,
                VBOUnmapResult,
                IBOCreateResult,
                IBOUpdateResult,
                VAOCreateResult,
                VAOSetAttributeResult,
                BufferDestroyResult
        > data;

        BufferExecuteResult() = default;

        BufferExecuteResult(const FBOCreateResult& r) : operationType(OperationType::FBO_CREATE), data(r) {}
        BufferExecuteResult(const FBOAttachTextureResult& r) : operationType(OperationType::FBO_ATTACH_TEXTURE), data(r) {}
        BufferExecuteResult(const FBOChangeResult& r) : operationType(OperationType::FBO_CHANGE), data(r) {}
        BufferExecuteResult(const FBOBlitResult& r) : operationType(OperationType::FBO_BLIT), data(r) {}
        BufferExecuteResult(const VBOCreateResult& r) : operationType(OperationType::VBO_CREATE), data(r) {}
        BufferExecuteResult(const VBOUpdateResult& r) : operationType(OperationType::VBO_UPDATE), data(r) {}
        BufferExecuteResult(const VBOMapResult& r) : operationType(OperationType::VBO_MAP), data(r) {}
        BufferExecuteResult(const VBOUnmapResult& r) : operationType(OperationType::VBO_UNMAP), data(r) {}
        BufferExecuteResult(const IBOCreateResult& r) : operationType(OperationType::IBO_CREATE), data(r) {}
        BufferExecuteResult(const IBOUpdateResult& r) : operationType(OperationType::IBO_UPDATE), data(r) {}
        BufferExecuteResult(const VAOCreateResult& r) : operationType(OperationType::VAO_CREATE), data(r) {}
        BufferExecuteResult(const VAOSetAttributeResult& r) : operationType(OperationType::VAO_SET_ATTRIBUTE), data(r) {}
        BufferExecuteResult(const BufferDestroyResult& r) : operationType(OperationType::BUFFER_DESTROY), data(r) {}
    };
}