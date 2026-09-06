#pragma once

#include <variant>
#include "RHI/RectRHI.h"

namespace BufferExecute {
    enum class BufferType {
        Framebuffer,
        VAO,
        VBO,
        IBO
    };

    enum class BindTarget {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    enum class AttachmentType {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    enum class BlitMask {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    enum class FilterMode {
        Nearest,
        Linear
    };

    enum class BufferUsage {
        Static,
        Dynamic,
        Stream,
        StaticRead,
        DynamicRead,
        StreamRead
    };

    enum class IndexType {
        UByte,
        UShort,
        UInt
    };

    enum class AttributeType {
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        UInt, UInt2, UInt3, UInt4,
        Matrix2x2, Matrix3x3, Matrix4x4
    };

    struct FBOCreateParams {
        BindTarget bindTarget;
        bool isGen;
        bool isBind;
    };

    struct FBOAttachTextureParams {
        unsigned int fboID;
        AttachmentType attachment;
        unsigned int textureID;
        int mipLevel;
        int layer;
    };

    struct FBOChangeParams {
        unsigned int fboID;
        bool isRebind;
        BindTarget newBindTarget;
        bool isCheckStatus;
        
        bool isChangeAttachment;
    };

    struct FBOBlitParams {
        unsigned int sourceFBO;
        unsigned int destFBO;
        
        RectRHI sourceRect;
        RectRHI destRect;
        
        BlitMask mask;
        FilterMode filter;
    };

    
    struct VBOCreateParams {
        const void* data;
        size_t size;
        BufferUsage usage;
        bool isGen;
        bool isBind;
    };

    struct VBOUpdateParams {
        unsigned int vboID;
        size_t offset;
        size_t size;
        const void* data;
        bool isFullUpdate;
    };

    struct VBOMapParams {
        unsigned int vboID;
        bool isReadOnly;
        bool isWriteOnly;
        bool isReadWrite;
        size_t offset;
        size_t length;
    };

    struct VBOUnmapParams {
        unsigned int vboID;
    };
    

    struct IBOCreateParams {
        const void* data;
        size_t count;
        IndexType indexType;
        BufferUsage usage;
        bool isGen;
        bool isBind;
    };

    struct IBOUpdateParams {
        unsigned int iboID;
        size_t offset;
        size_t count;
        const void* data;
        bool isFullUpdate;
    };


    struct VAOCreateParams {
        unsigned int vboID;
        unsigned int iboID;
        bool isGen;
        bool isBind;
    };

    struct VAOSetAttributeParams {
        unsigned int vaoID;
        unsigned int vboID;
        unsigned int index;
        AttributeType type;
        size_t offset;
        size_t stride;
        bool isNormalized;
        bool isInteger;
        bool isEnable;
        unsigned int divisor;
    };

    struct BufferDestroyParams {
        unsigned int bufferId;
        bool isDelete;
    };

    using BufferParams = std::variant<
        FBOCreateParams,
        FBOAttachTextureParams,
        FBOChangeParams,
        FBOBlitParams,

        VBOCreateParams,
        VBOUpdateParams,
        VBOMapParams,
        VBOUnmapParams,

        IBOCreateParams,
        IBOUpdateParams,

        VAOCreateParams,
        VAOSetAttributeParams,

        BufferDestroyParams
    >;
}