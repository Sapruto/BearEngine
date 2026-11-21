#include "RHI/Base/Contracts/DrawExecutorResult.h"
#include "RHI/Base/Handles.h"
#include "RHI/OpenGL/Executors/DrawExecutorGL.h"

#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ResourceGL.h"
#include "RHI/OpenGL/Executors/PipelineExecutorGL.h"
#include "RHI/OpenGL/Utilities.h"
#include "RHI/Base/Core/BaseDevice.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

using namespace RHI::Base;
using namespace RHI::Handles;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::Executors::PipelineExecutor;
using namespace RHI::OpenGL;
using namespace RHI::OpenGL::Utilities;

bool DrawExecutorGL::ApplyVBB(
    uint32_t vertexBufferCount,
    const VertexBufferBinding* vertexBuffers,
    ResourceGL& resourceGL,
    const PipelineGL* pipeline,
    std::string& errorMsg) {
    for (uint32_t i = 0; i < vertexBufferCount; ++i) {
        const auto& vb = vertexBuffers[i];
        BufferGL* buf = resourceGL.GetNativeAs<BufferGL, BufferHandle>(vb.buffer);
        if (!buf) {
            errorMsg = "Vertex buffer not found";
            return false;
        }

        GLsizei stride = 0;
        if (i < pipeline->state.vertexBindings.size()) {
            stride = static_cast<GLsizei>(pipeline->state.vertexBindings[i].stride);
        }

        glBindVertexBuffer(vertexBuffers[i].binding, buf->id,
                        static_cast<GLintptr>(vb.offset), stride);
    }
    return true;
}


DrawExecutorGL::DrawResult DrawExecutorGL::BindRenderTarget(RenderTargetHandle renderTargetHandle, BeginRenderPassParams params, ResourceGL& resourceGL) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    RenderTargetGL* rt = resourceGL.GetNativeAs<RenderTargetGL, RenderTargetHandle>(renderTargetHandle);
    if (!rt) {
        return DrawResult("RenderTarget not found");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);
    glViewport(params.renderArea.x, params.renderArea.y,
            params.renderArea.width, params.renderArea.height);
    glScissor(params.renderArea.x, params.renderArea.y,
            params.renderArea.width, params.renderArea.height);

    GLbitfield mask = 0;
    if (params.clearFlags.clearColor) {
        glClearColor(params.clearColorR, params.clearColorG,
                    params.clearColorB, params.clearColorA);
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (params.clearFlags.clearDepth) {
        glClearDepth(static_cast<GLdouble>(params.clearDepth));
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if (params.clearFlags.clearStencil) {
        glClearStencil(static_cast<GLint>(params.clearStencil));
        mask |= GL_STENCIL_BUFFER_BIT;
    }
    if (mask) {
        glClear(mask);
    }

    if (!CheckGLError("BeginRenderPass", errorMsg, errorCode)) {
        return DrawResult(errorMsg);
    }

    return DrawResult();
}

DrawExecutorGL::DrawResult DrawExecutorGL::BindTexture(TextureHandle textureTargetHandle,
                                                    BeginRenderPassParams params,
                                                    ResourceGL& resourceGL) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    TextureGL* tex = resourceGL.GetNativeAs<TextureGL, TextureHandle>(textureTargetHandle);
    if (!tex) return DrawResult("Texture not found");

    if (tex->fbo == 0 || tex->fboDirty) {
        if (tex->fbo == 0) glGenFramebuffers(1, &tex->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, tex->fbo);

        if (tex->isCube) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X, tex->id, 0);
        }
        else if (tex->is3D || tex->isArray) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    tex->id, 0, 0);
        }
        else if (tex->isMultisample) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D_MULTISAMPLE, tex->id, 0);
        }
        else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                tex->target, tex->id, 0);
        }

        if (tex->depthStencilTex) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_TEXTURE_2D, tex->depthStencilTex, 0);
        }

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            errorMsg = "Texture FBO incomplete (status=" + std::to_string(status) + ")";
            return DrawResult(errorMsg);
        }
        tex->fboDirty = false;
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, tex->fbo);
    }

    glViewport(params.renderArea.x, params.renderArea.y,
            params.renderArea.width, params.renderArea.height);
    glScissor(params.renderArea.x, params.renderArea.y,
            params.renderArea.width, params.renderArea.height);

    GLbitfield mask = 0;
    if (params.clearFlags.clearColor) {
        glClearColor(params.clearColorR, params.clearColorG,
                    params.clearColorB, params.clearColorA);
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (params.clearFlags.clearDepth) {
        glClearDepth(static_cast<GLdouble>(params.clearDepth));
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if (params.clearFlags.clearStencil) {
        glClearStencil(static_cast<GLint>(params.clearStencil));
        mask |= GL_STENCIL_BUFFER_BIT;
    }
    if (mask) glClear(mask);

    if (!CheckGLError("BeginRenderPass (Texture)", errorMsg, errorCode)) {
        return DrawResult(errorMsg);
    }
    return DrawResult();
}

DrawExecutorGL::DrawResult DrawExecutorGL::EndDrawRenderTarget(RenderTargetHandle renderTargetHandle,
                                                            [[maybe_unused]] EndRenderPassParams params,
                                                            ResourceGL& resourceGL) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    auto* rt = resourceGL.GetNativeAs<RenderTargetGL, RenderTargetHandle>(renderTargetHandle);
    if (rt && rt->samples != SampleCount::COUNT_1 && !rt->resolveTargets.empty()) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->fbo);

        for (size_t i = 0; i < rt->resolveTargets.size() && i < rt->colors.size(); ++i) {
            auto* dst = resourceGL.GetNativeAs<TextureGL>(rt->resolveTargets[i]);
            if (!dst) continue;

            GLuint resolveFbo = 0;
            glGenFramebuffers(1, &resolveFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo);

            GLenum attachment = GL_COLOR_ATTACHMENT0;
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, dst->target, dst->id, 0);
            glDrawBuffers(1, &attachment);

            glBlitFramebuffer(0, 0, rt->width, rt->height,
                            0, 0, dst->width, dst->height,
                            GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &resolveFbo);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!CheckGLError("EndRenderPass (RenderTarget)", errorMsg, errorCode)) {
        return DrawResult(errorMsg);
    }
    return DrawResult();
}

DrawExecutorGL::DrawResult DrawExecutorGL::EndDrawTexture(TextureHandle textureTargetHandle,
                                                        [[maybe_unused]] EndRenderPassParams params,
                                                        ResourceGL& resourceGL) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    TextureGL* tex = resourceGL.GetNativeAs<TextureGL, TextureHandle>(textureTargetHandle);
    if (!tex) return DrawResult("Texture not found");

    if (tex->isMultisample && tex->resolveTex != 0) {
        GLuint resolveFbo = 0;
        glGenFramebuffers(1, &resolveFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo);
        GLenum attachment = GL_COLOR_ATTACHMENT0;
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                            GL_TEXTURE_2D, tex->resolveTex, 0);
        glDrawBuffers(1, &attachment);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, tex->fbo);
        glBlitFramebuffer(0, 0, tex->width, tex->height,
                        0, 0, tex->width, tex->height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &resolveFbo);
    }

    if (tex->mipLevels > 1 && !tex->isMultisample) {
        glBindTexture(tex->target, tex->id);
        glGenerateMipmap(tex->target);
        glBindTexture(tex->target, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!CheckGLError("EndRenderPass (Texture)", errorMsg, errorCode)) {
        return DrawResult(errorMsg);
    }
    return DrawResult();
}


DrawExecuteResult DrawExecutorGL::ProcessDrawElementsImpl(BaseDevice& device, const DrawElementsParams& params) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    PipelineGL* pipeline = resourceGL.GetNativeAs<PipelineGL, PipelineHandle>(params.pipeline);
    if (!pipeline) {
        return DrawExecuteResult(DrawElementsResult{ params.pipeline, 0, 0, false, 0, false, "Pipeline not found" });
    }

    BufferGL* indexBuffer = resourceGL.GetNativeAs<BufferGL, BufferHandle>(params.indexBuffer);
    if (!indexBuffer) {
        return DrawExecuteResult(DrawElementsResult{ params.pipeline, 0, 0, false, 0, false, "Index buffer not found" });
    }

    if (pipelineExecutor) {
        pipelineExecutor->ApplyPipelineByHandle(device, params.pipeline);
    }

    glBindVertexArray(pipeline->vao);

    if (!ApplyVBB(params.vertexBufferCount, params.vertexBuffers,
                resourceGL, pipeline, errorMsg)) {
        return DrawExecuteResult(DrawElementsResult{
            params.pipeline, 0, 0, false, 0, false, errorMsg });
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);

    if (!CheckGLError("Bind", errorMsg, errorCode)) {
        return DrawExecuteResult(DrawElementsResult{ params.pipeline, 0, 0, false, 0, false, errorMsg });
    }

    GLenum mode = ToGL(pipeline->state.topology);
    GLenum indexType = ToGL(pipeline->state.indexFormat);

    unsigned int drawnCount = 0;
    unsigned int indexCount = 0;

    std::visit([&](const auto& d) {
        using T = std::decay_t<decltype(d)>;

        if constexpr (std::is_same_v<T, DrawElementsParams::Plain>) {
            glDrawElementsBaseVertex(
                mode, d.indexCount, indexType,
                (const void*)(uintptr_t)(params.indexBufferOffset + d.indexOffset),
                d.baseVertex);
            drawnCount = d.indexCount;
            indexCount = d.indexCount;
        }
        else if constexpr (std::is_same_v<T, DrawElementsParams::Range>) {
            glDrawRangeElementsBaseVertex(
                mode, d.minIndex, d.maxIndex, d.indexCount, indexType,
                (const void*)(uintptr_t)(params.indexBufferOffset + d.indexOffset),
                d.baseVertex);
            drawnCount = d.indexCount;
            indexCount = d.indexCount;
        }
        else if constexpr (std::is_same_v<T, DrawElementsParams::Instanced>) {
            glDrawElementsInstancedBaseVertexBaseInstance(
                mode, d.indexCount, indexType,
                (const void*)(uintptr_t)(params.indexBufferOffset + d.indexOffset),
                d.instanceCount, d.baseVertex, d.baseInstance);
            drawnCount = d.indexCount * d.instanceCount;
            indexCount = d.indexCount;
        }
        else if constexpr (std::is_same_v<T, DrawElementsParams::RangeInstanced>) {
            glDrawElementsInstancedBaseVertexBaseInstance(
                mode, d.indexCount, indexType,
                (const void*)(uintptr_t)(params.indexBufferOffset + d.indexOffset),
                d.instanceCount, d.baseVertex, d.baseInstance);
            drawnCount = d.indexCount * d.instanceCount;
            indexCount = d.indexCount;
        }
    }, params.data);

    if (!CheckGLError("glDrawElements*", errorMsg, errorCode)) {
        return DrawExecuteResult(DrawElementsResult{ params.pipeline, indexCount, 0, false, 0, false, errorMsg });
    }

    DrawElementsResult result;
    result.pipeline = params.pipeline;
    result.indexCount = indexCount;
    result.drawnCount = drawnCount;
    result.isInstanced = std::holds_alternative<DrawElementsParams::Instanced>(params.data)
                    || std::holds_alternative<DrawElementsParams::RangeInstanced>(params.data);
    result.instanceCount = 0;
    result.success = true;
    result.errorMessage = "";

    return DrawExecuteResult(result);
}

DrawExecuteResult DrawExecutorGL::ProcessDrawArraysImpl(BaseDevice& device, const DrawArraysParams& params) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    PipelineGL* pipeline = resourceGL.GetNativeAs<PipelineGL, PipelineHandle>(params.pipeline);
    if (!pipeline) {
        return DrawExecuteResult(DrawArraysResult{ params.pipeline, 0, 0, 0, false, "Pipeline not found" });
    }

    if (pipelineExecutor) {
        pipelineExecutor->ApplyPipelineByHandle(device, params.pipeline);
    }

    if (!ApplyVBB(params.vertexBufferCount, params.vertexBuffers,
                resourceGL, pipeline, errorMsg)) {
        return DrawExecuteResult(DrawElementsResult{
            params.pipeline, 0, 0, false, 0, false, errorMsg });
    }

    if (!CheckGLError("Bind vertex buffers", errorMsg, errorCode)) {
        return DrawExecuteResult(DrawArraysResult{ params.pipeline, 0, 0, 0, false, errorMsg });
    }

    GLenum mode = ToGL(pipeline->state.topology);

    std::visit([&](const auto& d) {
        using T = std::decay_t<decltype(d)>;

        if constexpr (std::is_same_v<T, DrawArraysParams::Plain>) {
            glDrawArrays(mode, d.first, d.count);
        }
        else if constexpr (std::is_same_v<T, DrawArraysParams::Instanced>) {
            glDrawArraysInstancedBaseInstance(
                mode, d.first, d.count, d.instanceCount, d.baseInstance);
        }
    }, params.data);

    if (!CheckGLError("glDrawArrays*", errorMsg, errorCode)) {
        return DrawExecuteResult(DrawArraysResult{ params.pipeline, 0, 0, 0, false, errorMsg });
    }

    DrawArraysResult result;
    result.pipeline = params.pipeline;
    result.first = 0;
    result.count = 0;
    result.drawnCount = 0;
    result.success = true;
    result.errorMessage = "";

    std::visit([&](const auto& d) {
        using T = std::decay_t<decltype(d)>;
        if constexpr (std::is_same_v<T, DrawArraysParams::Plain>) {
            result.first = d.first;
            result.count = d.count;
            result.drawnCount = d.count;
        }
        else if constexpr (std::is_same_v<T, DrawArraysParams::Instanced>) {
            result.first = d.first;
            result.count = d.count;
            result.drawnCount = d.count * d.instanceCount;
        }
    }, params.data);

    return DrawExecuteResult(result);
}

DrawExecuteResult DrawExecutorGL::ProcessBeginRenderPassImpl(BaseDevice& device, const BeginRenderPassParams& params) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    DrawResult result;
    
    std::visit([&](const auto& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, RenderTargetHandle>) {
            result = BindRenderTarget(target, params, resourceGL);
        }
        else if constexpr (std::is_same_v<T, TextureHandle>) {
            result = BindTexture(target, params, resourceGL);
        }
        else {
            result = DrawResult("Target is std::monostate");
        }
    }, params.target);
    return DrawExecuteResult(BeginRenderPassResult{ params.target, result.success, result.errorMsg });
}

DrawExecuteResult DrawExecutorGL::ProcessEndRenderPassImpl(BaseDevice& device, const EndRenderPassParams& params) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    DrawResult result;

    std::visit([&](const auto& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, RenderTargetHandle>) {
            result = EndDrawRenderTarget(target, params, resourceGL);
        }
        else if constexpr (std::is_same_v<T, TextureHandle>) {
            result = EndDrawTexture(target, params, resourceGL);
        }
        else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }, params.target);

    if (!result.success) {
        return DrawExecuteResult(EndRenderPassResult{ false, result.errorMsg });
    }
    return DrawExecuteResult(EndRenderPassResult{ true, "" });
}

DrawExecuteResult DrawExecutorGL::ProcessSetViewportImpl([[maybe_unused]] BaseDevice& device, const SetViewportParams& params) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    glViewport(params.viewport.x, params.viewport.y,
            params.viewport.width, params.viewport.height);
    glDepthRange(static_cast<GLdouble>(params.minDepth),
                static_cast<GLdouble>(params.maxDepth));

    if (!CheckGLError("glViewport", errorMsg, errorCode)) {
        return DrawExecuteResult(SetViewportResult{ params.viewport, false, errorMsg });
    }

    return DrawExecuteResult(SetViewportResult{ params.viewport, true, "" });
}

DrawExecuteResult DrawExecutorGL::ProcessSetScissorImpl([[maybe_unused]] BaseDevice& device, const SetScissorParams& params) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    glEnable(GL_SCISSOR_TEST);
    glScissor(params.scissor.x, params.scissor.y,
            params.scissor.width, params.scissor.height);

    if (!CheckGLError("glScissor", errorMsg, errorCode)) {
        return DrawExecuteResult(SetScissorResult{ params.scissor, false, errorMsg });
    }

    return DrawExecuteResult(SetScissorResult{ params.scissor, true, "" });
}

DrawExecuteResult DrawExecutorGL::ProcessSetBlendConstantsImpl([[maybe_unused]] BaseDevice& device, const SetBlendConstantsParams& params) {
    std::string errorMsg;
    GLenum errorCode = GL_NO_ERROR;

    glBlendColor(params.r, params.g, params.b, params.a);

    if (!CheckGLError("glBlendColor", errorMsg, errorCode)) {
        return DrawExecuteResult(SetBlendConstantsResult{ false, errorMsg });
    }

    return DrawExecuteResult(SetBlendConstantsResult{ true, "" });
}

DrawExecuteResult DrawExecutorGL::ProcessBindUniformBufferImpl(BaseDevice& device, const BindUniformBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    BufferGL* buf = resourceGL.GetNativeAs<BufferGL, BufferHandle>(p.buffer);
    if (!buf) {
        DrawExecuteResult result = DrawExecuteResult(BindUniformBufferResult());
        result.success = false;
        result.errorMessage = "Uniform buffer not found";
        return result;
    }

    if (p.size == 0) {
        glBindBufferBase(GL_UNIFORM_BUFFER, p.binding, buf->id);
    }
    else {
        glBindBufferRange(GL_UNIFORM_BUFFER, p.binding, buf->id,
                        static_cast<GLintptr>(p.offset),
                        static_cast<GLsizeiptr>(p.size));
    }

    return DrawExecuteResult(BindUniformBufferResult());
}

DrawExecuteResult DrawExecutorGL::ProcessBindStorageBufferImpl(BaseDevice& device, const BindStorageBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    BufferGL* buf = resourceGL.GetNativeAs<BufferGL, BufferHandle>(p.buffer);
    if (!buf) {
        DrawExecuteResult result = DrawExecuteResult(BindStorageBufferResult());
        result.success = false;
        result.errorMessage = "Storage buffer not found";
        return result;
    }

    if (p.size == 0) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, p.binding, buf->id);
    }
    else {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, p.binding, buf->id,
                            static_cast<GLintptr>(p.offset),
                            static_cast<GLsizeiptr>(p.size));
    }

    return DrawExecuteResult(BindStorageBufferResult());
}

bool DrawExecutorGL::IsValid() {
    return glfwGetCurrentContext() != nullptr;
}