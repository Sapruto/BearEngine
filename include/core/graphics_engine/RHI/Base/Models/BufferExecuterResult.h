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
    };

    struct FBOAttachTextureResult {
        unsigned int fboID;
        unsigned int textureID;
        bool isAttached;
        int attachmentPoint;
        std::string errorMessage;
    };

    struct FBOChangeResult {
        unsigned int fboID;
        bool isRebound;
        bool statusValid;
        bool isComplete;
        std::string statusMessage;
    };

    struct FBOBlitResult {
        unsigned int sourceFBO;
        unsigned int destFBO;
        bool isBlitted;
        size_t bytesTransferred;
        float transferTimeMs;
        std::string errorMessage;
    };

    struct VBOCreateResult {
        unsigned int vboID;
        size_t allocatedSize;
        bool isGenerated;
        bool isBound;
        BufferUsage actualUsage;
        std::string errorMessage;
    };

    struct VBOUpdateResult {
        unsigned int vboID;
        size_t bytesUpdated;
        size_t totalSize;
        bool isFullUpdate;
        bool success;
        std::string errorMessage;
    };

    struct VBOMapResult {
        unsigned int vboID;
        void* mappedPtr;
        size_t mappedSize;
        bool isMapped;
        bool isReadOnly;
        bool isWriteOnly;
        std::string errorMessage;
    };

    struct VBOUnmapResult {
        unsigned int vboID;
        bool isUnmapped;
        bool dataFlushed;
        std::string errorMessage;
    };

    struct IBOCreateResult {
        unsigned int iboID;
        size_t indexCount;
        IndexType indexType;
        bool isGenerated;
        bool isBound;
        BufferUsage actualUsage;
        std::string errorMessage;
    };

    struct IBOUpdateResult {
        unsigned int iboID;
        size_t indicesUpdated;
        size_t totalIndices;
        bool isFullUpdate;
        bool success;
        std::string errorMessage;
    };

    struct VAOCreateResult {
        unsigned int vaoID;
        unsigned int vboID;
        unsigned int iboID;
        bool isGenerated;
        bool isBound;
        bool isValid;
        std::string errorMessage;
    };

    struct VAOSetAttributeResult {
        unsigned int vaoID;
        unsigned int attributeIndex;
        bool isSet;
        bool isEnabled;
        size_t stride;
        size_t offset;
        std::string errorMessage;
    };

    struct BufferDestroyResult {
        unsigned int bufferId;
        bool isDestroyed;
        bool resourcesFreed;
        std::string errorMessage;
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
    };
}