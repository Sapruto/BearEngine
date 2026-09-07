#pragma once

#include <variant>
#include "any"
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
        ReadWrite,
        Undefined
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

    enum class ParamType : uint8_t {
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
        COUNT
    };

    struct BaseBufferParams {
    private:
        ParamType type;

    protected:
        void SetType(ParamType newType) { type = newType; }

    public:
        BaseBufferParams() : type(ParamType::FBO_CREATE) {}
        virtual ~BaseBufferParams() = default;
        
        ParamType GetType() const { return type; }
    };

    struct FBOCreateParams : public BaseBufferParams {
        BindTarget bindTarget;
        bool isBind;

        FBOCreateParams() {
            SetType(ParamType::FBO_CREATE);
        }
    };

    struct FBOAttachTextureParams : public BaseBufferParams {
        unsigned int fboID;
        AttachmentType attachment;
        unsigned int textureID;
        int mipLevel;
        int layer;

        FBOAttachTextureParams() {
            SetType(ParamType::FBO_ATTACH_TEXTURE);
        }
    };

    struct FBOChangeParams : public BaseBufferParams {
        unsigned int fboID;
        bool isRebind;
        BindTarget newBindTarget{BindTarget::Undefined};
        bool isCheckStatus;
        bool isChangeAttachment;

        FBOChangeParams() {
            SetType(ParamType::FBO_CHANGE);
        }
    };

    struct FBOBlitParams : public BaseBufferParams {
        unsigned int sourceFBO;
        unsigned int destFBO;
        RectRHI sourceRect;
        RectRHI destRect;
        BlitMask mask;
        FilterMode filter;

        FBOBlitParams() {
            SetType(ParamType::FBO_BLIT);
        }
    };

    struct VBOCreateParams : public BaseBufferParams {
        const void* data;
        size_t size;
        BufferUsage usage;
        bool isGen;
        bool isBind;

        VBOCreateParams() {
            SetType(ParamType::VBO_CREATE);
        }
    };

    struct VBOUpdateParams : public BaseBufferParams {
        unsigned int vboID;
        size_t offset;
        size_t size;
        const void* data;
        bool isFullUpdate;

        VBOUpdateParams() {
            SetType(ParamType::VBO_UPDATE);
        }
    };

    struct VBOMapParams : public BaseBufferParams {
        unsigned int vboID;
        bool isReadOnly;
        bool isWriteOnly;
        bool isReadWrite;
        size_t offset;
        size_t length;

        VBOMapParams() {
            SetType(ParamType::VBO_MAP);
        }
    };

    struct VBOUnmapParams : public BaseBufferParams {
        unsigned int vboID;

        VBOUnmapParams() {
            SetType(ParamType::VBO_UNMAP);
        }
    };

    struct IBOCreateParams : public BaseBufferParams {
        const void* data;
        size_t count;
        IndexType indexType;
        BufferUsage usage;
        bool isGen;
        bool isBind;

        IBOCreateParams() {
            SetType(ParamType::IBO_CREATE);
        }
    };

    struct IBOUpdateParams : public BaseBufferParams {
        unsigned int iboID;
        size_t offset;
        size_t count;
        IndexType indexType;
        const std::variant<std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>> data;
        bool isFullUpdate;

        IBOUpdateParams() {
            SetType(ParamType::IBO_UPDATE);
        }
    };

    struct VAOCreateParams : public BaseBufferParams {
        unsigned int vboID;
        unsigned int iboID;
        bool isGen;
        bool isBind;

        VAOCreateParams() {
            SetType(ParamType::VAO_CREATE);
        }
    };

    struct VAOSetAttributeParams : public BaseBufferParams {
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

        VAOSetAttributeParams() {
            SetType(ParamType::VAO_SET_ATTRIBUTE);
        }
    };

    struct BufferDestroyParams : public BaseBufferParams {
        unsigned int bufferId;
        BufferType bufferType;

        BufferDestroyParams() {
            SetType(ParamType::BUFFER_DESTROY);
        }
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