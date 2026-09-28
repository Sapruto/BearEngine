#pragma once

#include <variant>
#include <vector>
#include <cstdint>
#include <any>
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
        BASE,
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
        BaseBufferParams() : type(ParamType::BASE) {}
        ParamType GetType() const { return type; }
    };

    struct FBOCreateParams : public BaseBufferParams {
        BindTarget bindTarget;
        bool isBind;

        FBOCreateParams() : bindTarget(BindTarget::Undefined), isBind(false) {
            SetType(ParamType::FBO_CREATE);
        }

        FBOCreateParams(const BindTarget& target, bool isBind)
            : bindTarget(target), isBind(isBind) {
            SetType(ParamType::FBO_CREATE);
        }
    };

    struct FBOAttachTextureParams : public BaseBufferParams {
        unsigned int fboID;
        AttachmentType attachment;
        unsigned int textureID;
        int mipLevel;
        int layer;

        FBOAttachTextureParams() : fboID(0), attachment(AttachmentType::Color),
                                  textureID(0), mipLevel(0), layer(0) {
            SetType(ParamType::FBO_ATTACH_TEXTURE);
        }

        FBOAttachTextureParams(unsigned int fboID, AttachmentType attachment,
                               unsigned int textureID, int mipLevel, int layer)
            : fboID(fboID), attachment(attachment), textureID(textureID),
              mipLevel(mipLevel), layer(layer) {
            SetType(ParamType::FBO_ATTACH_TEXTURE);
        }
    };

    struct FBOChangeParams : public BaseBufferParams {
        unsigned int fboID;
        bool isRebind;
        BindTarget newBindTarget{BindTarget::Undefined};
        bool isCheckStatus;
        bool isChangeAttachment;

        FBOChangeParams() : fboID(0), isRebind(false), isCheckStatus(false),
                            isChangeAttachment(false) {
            SetType(ParamType::FBO_CHANGE);
        }

        FBOChangeParams(unsigned int fboID, bool isRebind, BindTarget newBindTarget,
                        bool isCheckStatus, bool isChangeAttachment)
            : fboID(fboID), isRebind(isRebind), newBindTarget(newBindTarget),
              isCheckStatus(isCheckStatus), isChangeAttachment(isChangeAttachment) {
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

        FBOBlitParams() : sourceFBO(0), destFBO(0), mask(BlitMask::Color),
                          filter(FilterMode::Nearest) {
            SetType(ParamType::FBO_BLIT);
        }

        FBOBlitParams(unsigned int sourceFBO, unsigned int destFBO,
                      const RectRHI& sourceRect, const RectRHI& destRect,
                      BlitMask mask, FilterMode filter)
            : sourceFBO(sourceFBO), destFBO(destFBO), sourceRect(sourceRect),
              destRect(destRect), mask(mask), filter(filter) {
            SetType(ParamType::FBO_BLIT);
        }
    };

    struct VBOCreateParams : public BaseBufferParams {
        const void* data;
        size_t size;
        BufferUsage usage;
        bool isGen;
        bool isBind;

        VBOCreateParams() : data(nullptr), size(0), usage(BufferUsage::Static),
                            isGen(true), isBind(true) {
            SetType(ParamType::VBO_CREATE);
        }

        VBOCreateParams(const void* data, size_t size, BufferUsage usage,
                        bool isGen = true, bool isBind = true)
            : data(data), size(size), usage(usage), isGen(isGen), isBind(isBind) {
            SetType(ParamType::VBO_CREATE);
        }
    };

    struct VBOUpdateParams : public BaseBufferParams {
        unsigned int vboID;
        size_t offset;
        size_t size;
        const void* data;
        bool isFullUpdate;

        VBOUpdateParams() : vboID(0), offset(0), size(0), data(nullptr),
                            isFullUpdate(false) {
            SetType(ParamType::VBO_UPDATE);
        }

        VBOUpdateParams(unsigned int vboID, size_t offset, size_t size,
                        const void* data, bool isFullUpdate)
            : vboID(vboID), offset(offset), size(size), data(data),
              isFullUpdate(isFullUpdate) {
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

        VBOMapParams() : vboID(0), isReadOnly(false), isWriteOnly(false),
                         isReadWrite(false), offset(0), length(0) {
            SetType(ParamType::VBO_MAP);
        }

        VBOMapParams(unsigned int vboID, bool isReadOnly, bool isWriteOnly,
                     bool isReadWrite, size_t offset, size_t length)
            : vboID(vboID), isReadOnly(isReadOnly), isWriteOnly(isWriteOnly),
              isReadWrite(isReadWrite), offset(offset), length(length) {
            SetType(ParamType::VBO_MAP);
        }
    };

    struct VBOUnmapParams : public BaseBufferParams {
        unsigned int vboID;

        VBOUnmapParams() : vboID(0) {
            SetType(ParamType::VBO_UNMAP);
        }

        explicit VBOUnmapParams(unsigned int vboID) : vboID(vboID) {
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

        IBOCreateParams() : data(nullptr), count(0), indexType(IndexType::UInt),
                            usage(BufferUsage::Static), isGen(true), isBind(true) {
            SetType(ParamType::IBO_CREATE);
        }

        IBOCreateParams(const void* data, size_t count, IndexType indexType,
                        BufferUsage usage, bool isGen = true, bool isBind = true)
            : data(data), count(count), indexType(indexType), usage(usage),
              isGen(isGen), isBind(isBind) {
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

        IBOUpdateParams() : iboID(0), offset(0), count(0), indexType(IndexType::UInt),
                            data(std::vector<uint8_t>{}), isFullUpdate(false) {
            SetType(ParamType::IBO_UPDATE);
        }

        IBOUpdateParams(unsigned int iboID, size_t offset, size_t count,
                        IndexType indexType,
                        const std::variant<std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>>& data,
                        bool isFullUpdate)
            : iboID(iboID), offset(offset), count(count), indexType(indexType),
              data(data), isFullUpdate(isFullUpdate) {
            SetType(ParamType::IBO_UPDATE);
        }
    };

    struct VAOCreateParams : public BaseBufferParams {
        unsigned int vboID;
        unsigned int iboID;
        bool isGen;
        bool isBind;

        VAOCreateParams() : vboID(0), iboID(0), isGen(true), isBind(true) {
            SetType(ParamType::VAO_CREATE);
        }

        VAOCreateParams(unsigned int vboID, unsigned int iboID,
                        bool isGen = true, bool isBind = true)
            : vboID(vboID), iboID(iboID), isGen(isGen), isBind(isBind) {
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

        VAOSetAttributeParams()
            : vaoID(0), vboID(0), index(0), type(AttributeType::Float),
              offset(0), stride(0), isNormalized(false), isInteger(false),
              isEnable(true), divisor(0) {
            SetType(ParamType::VAO_SET_ATTRIBUTE);
        }

        VAOSetAttributeParams(unsigned int vaoID, unsigned int vboID, unsigned int index,
                              AttributeType type, size_t offset, size_t stride,
                              bool isNormalized, bool isInteger, bool isEnable,
                              unsigned int divisor = 0)
            : vaoID(vaoID), vboID(vboID), index(index), type(type),
              offset(offset), stride(stride), isNormalized(isNormalized),
              isInteger(isInteger), isEnable(isEnable), divisor(divisor) {
            SetType(ParamType::VAO_SET_ATTRIBUTE);
        }
    };

    struct BufferDestroyParams : public BaseBufferParams {
        unsigned int bufferId;
        BufferType bufferType;

        BufferDestroyParams() : bufferId(0), bufferType(BufferType::VBO) {
            SetType(ParamType::BUFFER_DESTROY);
        }

        BufferDestroyParams(unsigned int bufferId, BufferType bufferType)
            : bufferId(bufferId), bufferType(bufferType) {
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