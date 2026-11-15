#pragma once

#include <cstdint>
#include <variant>

#include "RHI/RectRHI.h"
#include "RHI/Base/Handles.h"

namespace RHI::Executors::DrawExecutor {
    struct VertexBufferBinding {
        Handles::BufferHandle buffer{};
        uint64_t offset = 0;
        uint64_t binding = 0;

        VertexBufferBinding() = default;

        VertexBufferBinding(Handles::BufferHandle b,
                            uint64_t bind,
                            uint64_t off = 0)
            : buffer(b)
            , offset(off)
            , binding(bind) {}
    };

    struct BaseDrawParams {
        Handles::PipelineHandle pipeline{};
        VertexBufferBinding* vertexBuffers = nullptr;
        uint32_t vertexBufferCount = 0;
        Handles::BufferHandle indexBuffer{};
        uint64_t indexBufferOffset = 0;

        BaseDrawParams() = default;

        BaseDrawParams(Handles::PipelineHandle p,
                        VertexBufferBinding* vbs,
                        uint32_t vbCount,
                        Handles::BufferHandle ib = {},
                        uint64_t ibOffset = 0)
            : pipeline(p)
            , vertexBuffers(vbs)
            , vertexBufferCount(vbCount)
            , indexBuffer(ib)
            , indexBufferOffset(ibOffset) {}
    };

    struct DrawArraysParams : public BaseDrawParams {
        struct Plain {
            uint32_t first = 0;
            uint32_t count = 0;

            Plain() = default;
            Plain(uint32_t f, uint32_t c) : first(f), count(c) {}
        };
        struct Instanced {
            uint32_t first = 0;
            uint32_t count = 0;
            uint32_t instanceCount = 0;
            uint32_t baseInstance = 0;

            Instanced() = default;
            Instanced(uint32_t f, uint32_t c, uint32_t ic, uint32_t bi = 0)
                : first(f), count(c), instanceCount(ic), baseInstance(bi) {}
        };

        std::variant<Plain, Instanced> data{Plain{}};

        DrawArraysParams() = default;

        DrawArraysParams(Handles::PipelineHandle p,
                        VertexBufferBinding* vbs,
                        uint32_t vbCount,
                        Plain plain)
            : BaseDrawParams(p, vbs, vbCount)
            , data(plain) {}

        DrawArraysParams(Handles::PipelineHandle p,
                        VertexBufferBinding* vbs,
                        uint32_t vbCount,
                        Instanced inst)
            : BaseDrawParams(p, vbs, vbCount)
            , data(inst) {}
    };

    struct DrawElementsParams : public BaseDrawParams {
        struct Plain {
            uint32_t indexCount = 0;
            uint32_t indexOffset = 0;
            uint32_t baseVertex = 0;

            Plain() = default;
            Plain(uint32_t count, uint32_t off = 0, uint32_t bv = 0)
                : indexCount(count), indexOffset(off), baseVertex(bv) {}
        };
        struct Range {
            uint32_t indexCount = 0;
            uint32_t indexOffset = 0;
            uint32_t minIndex = 0;
            uint32_t maxIndex = 0;
            uint32_t baseVertex = 0;

            Range() = default;
            Range(uint32_t count, uint32_t minI, uint32_t maxI,
                    uint32_t off = 0, uint32_t bv = 0)
                : indexCount(count), indexOffset(off)
                , minIndex(minI), maxIndex(maxI), baseVertex(bv) {}
        };
        struct Instanced {
            uint32_t indexCount = 0;
            uint32_t indexOffset = 0;
            uint32_t baseVertex = 0;
            uint32_t instanceCount = 0;
            uint32_t baseInstance = 0;

            Instanced() = default;
            Instanced(uint32_t count, uint32_t instCount,
                        uint32_t off = 0, uint32_t bv = 0, uint32_t bi = 0)
                : indexCount(count), indexOffset(off), baseVertex(bv)
                , instanceCount(instCount), baseInstance(bi) {}
        };
        struct RangeInstanced {
            uint32_t indexCount = 0;
            uint32_t indexOffset = 0;
            uint32_t minIndex = 0;
            uint32_t maxIndex = 0;
            uint32_t baseVertex = 0;
            uint32_t instanceCount = 0;
            uint32_t baseInstance = 0;

            RangeInstanced() = default;
            RangeInstanced(uint32_t count, uint32_t minI, uint32_t maxI,
                            uint32_t instCount,
                            uint32_t off = 0, uint32_t bv = 0, uint32_t bi = 0)
                : indexCount(count), indexOffset(off)
                , minIndex(minI), maxIndex(maxI), baseVertex(bv)
                , instanceCount(instCount), baseInstance(bi) {}
        };

        std::variant<Plain, Range, Instanced, RangeInstanced> data{Plain{}};

        DrawElementsParams() = default;

        DrawElementsParams(Handles::PipelineHandle p,
                            VertexBufferBinding* vbs,
                            uint32_t vbCount,
                            Handles::BufferHandle ib,
                            Plain plain,
                            uint64_t ibOffset = 0)
            : BaseDrawParams(p, vbs, vbCount, ib, ibOffset)
            , data(plain) {}

        DrawElementsParams(Handles::PipelineHandle p,
                            VertexBufferBinding* vbs,
                            uint32_t vbCount,
                            Handles::BufferHandle ib,
                            Range range,
                            uint64_t ibOffset = 0)
            : BaseDrawParams(p, vbs, vbCount, ib, ibOffset)
            , data(range) {}

        DrawElementsParams(Handles::PipelineHandle p,
                            VertexBufferBinding* vbs,
                            uint32_t vbCount,
                            Handles::BufferHandle ib,
                            Instanced inst,
                            uint64_t ibOffset = 0)
            : BaseDrawParams(p, vbs, vbCount, ib, ibOffset)
            , data(inst) {}

        DrawElementsParams(Handles::PipelineHandle p,
                            VertexBufferBinding* vbs,
                            uint32_t vbCount,
                            Handles::BufferHandle ib,
                            RangeInstanced ri,
                            uint64_t ibOffset = 0)
            : BaseDrawParams(p, vbs, vbCount, ib, ibOffset)
            , data(ri) {}
    };

    struct BeginRenderPassParams {
        struct ClearFlags {
            bool clearColor = false;
            bool clearDepth = false;
            bool clearStencil = false;

            ClearFlags() {}
            ClearFlags(bool c, bool d, bool s)
                : clearColor(c), clearDepth(d), clearStencil(s) {}
        };

        Handles::RenderTargetHandle renderTarget{};
        RectRHI renderArea{};
        float clearColorR = 0.0f;
        float clearColorG = 0.0f;
        float clearColorB = 0.0f;
        float clearColorA = 1.0f;
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;
        ClearFlags clearFlags;

        BeginRenderPassParams() = default;

        BeginRenderPassParams(Handles::RenderTargetHandle rt,
                            RectRHI area,
                            ClearFlags flags = {},
                            float r = 0.0f, float g = 0.0f,
                            float b = 0.0f, float a = 1.0f,
                            float depth = 1.0f, uint32_t stencil = 0)
            : renderTarget(rt)
            , renderArea(area)
            , clearColorR(r), clearColorG(g), clearColorB(b), clearColorA(a)
            , clearDepth(depth)
            , clearStencil(stencil)
            , clearFlags(flags) {}
    };

    struct EndRenderPassParams {};

    struct SetViewportParams {
        RectRHI viewport{};
        float minDepth = 0.0f;
        float maxDepth = 1.0f;

        SetViewportParams() = default;

        SetViewportParams(RectRHI vp, float mn = 0.0f, float mx = 1.0f)
            : viewport(vp), minDepth(mn), maxDepth(mx) {}
    };

    struct SetScissorParams {
        RectRHI scissor{};

        SetScissorParams() = default;
        explicit SetScissorParams(RectRHI s) : scissor(s) {}
    };

    struct SetBlendConstantsParams {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 0.0f;

        SetBlendConstantsParams() = default;
        SetBlendConstantsParams(float rr, float gg, float bb, float aa)
            : r(rr), g(gg), b(bb), a(aa) {}
    };

    struct BindUniformBufferParams {
        Handles::BufferHandle buffer{};
        uint32_t binding = 0;
        uint32_t set = 0;
        uint64_t offset = 0;
        uint64_t size = 0;

        BindUniformBufferParams() = default;

        BindUniformBufferParams(Handles::BufferHandle b,
                                uint32_t bind,
                                uint32_t s = 0,
                                uint64_t off = 0,
                                uint64_t sz = 0)
            : buffer(b), binding(bind), set(s), offset(off), size(sz) {}
    };

    struct BindStorageBufferParams {
        Handles::BufferHandle buffer{};
        uint32_t binding = 0;
        uint32_t set = 0;
        uint64_t offset = 0;
        uint64_t size = 0;

        BindStorageBufferParams() = default;

        BindStorageBufferParams(Handles::BufferHandle b,
                                uint32_t bind,
                                uint32_t s = 0,
                                uint64_t off = 0,
                                uint64_t sz = 0)
            : buffer(b), binding(bind), set(s), offset(off), size(sz) {}
    };

    using DrawParams = std::variant<
        DrawArraysParams,
        DrawElementsParams,
        BeginRenderPassParams,
        EndRenderPassParams,
        SetViewportParams,
        SetScissorParams,
        SetBlendConstantsParams,
        BindUniformBufferParams,
        BindStorageBufferParams
    >;
}