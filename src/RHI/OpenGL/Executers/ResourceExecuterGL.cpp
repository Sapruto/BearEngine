#include "Logger/Logger.h"
#include "RHI/OpenGL/Executors/ResourceExecutorGL.h"

#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ResourceGL.h"
#include "RHI/OpenGL/HandlesGL.h"
#include "RHI/OpenGL/Utilities.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace RHI;
using namespace RHI::Base;
using namespace RHI::Handles;
using namespace RHI::OpenGL;
using namespace RHI::OpenGL::Utilities;
using namespace RHI::Executors::ResourceExecutor;

ResourceExecuteResult ResourceExecutorGL::ProcessCreateBufferImpl(BaseDevice& device, const CreateBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    std::string errorMsg;
    if (!ValidateBufferDesc(p.desc, errorMsg)) {
        ResourceExecuteResult result(CreateBufferResult{});
        result.ProcessFailed(errorMsg);
        return result;
    }

    BufferGL buf;
    buf.target = ToGLTarget(p.desc.usage);

    glGenBuffers(1, &buf.id);
    if (buf.id == 0) {
        ResourceExecuteResult result(CreateBufferResult{});
        result.ProcessFailed("glGenBuffers failed");
        return result;
    }

    glBindBuffer(buf.target, buf.id);
    glBufferData(buf.target, static_cast<GLsizeiptr>(p.desc.size), nullptr, ToGLUsageHint(p.desc.usage));
    glBindBuffer(buf.target, 0);

    BufferHandle handle = resourceGL.Create<BufferHandle>(std::move(buf));
    return ResourceExecuteResult(CreateBufferResult{ handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessUpdateBufferImpl(BaseDevice& device, const UpdateBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    auto* buf = resourceGL.GetNativeAs<BufferGL>(p.handle);
    if (!buf) {
        ResourceExecuteResult result(UpdateBufferResult{ p.handle, 0 });
        result.ProcessFailed("Buffer not found");
        return result;
    }
    if (p.data.empty()) {
        ResourceExecuteResult result(UpdateBufferResult{ p.handle, 0 });
        result.ProcessFailed("Empty data");
        return result;
    }

    glBindBuffer(buf->target, buf->id);
    glBufferSubData(buf->target, static_cast<GLintptr>(p.offset),
                    static_cast<GLsizeiptr>(p.data.size()), p.data.data());
    glBindBuffer(buf->target, 0);

    return ResourceExecuteResult(UpdateBufferResult{ p.handle, p.data.size() });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCopyBufferImpl(BaseDevice& device, const CopyBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    auto* src = resourceGL.GetNativeAs<BufferGL>(p.src);
    auto* dst = resourceGL.GetNativeAs<BufferGL>(p.dst);
    if (!src || !dst) {
        ResourceExecuteResult result(CopyBufferResult{ p.src, p.dst, 0 });
        result.ProcessFailed("Buffer not found");
        return result;
    }
    if (p.size == 0) {
        ResourceExecuteResult result(CopyBufferResult{ p.src, p.dst, 0 });
        result.ProcessFailed("Copy size is 0");
        return result;
    }

    glBindBuffer(GL_COPY_READ_BUFFER, src->id);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dst->id);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                        static_cast<GLintptr>(p.srcOffset),
                        static_cast<GLintptr>(p.dstOffset),
                        static_cast<GLsizeiptr>(p.size));
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    return ResourceExecuteResult(CopyBufferResult{ p.src, p.dst, p.size });
}

ResourceExecuteResult ResourceExecutorGL::ProcessDestroyBufferImpl(BaseDevice& device, const DestroyBufferParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    bool exists = resourceGL.ContainsId(p.handle.id);
    if (exists) resourceGL.RemoveById(p.handle.id);

    if (!exists) {
        ResourceExecuteResult result(DestroyBufferResult{ p.handle });
        result.ProcessFailed("Buffer not found");
        return result;
    }

    return ResourceExecuteResult(DestroyBufferResult{ p.handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCreateShaderImpl(BaseDevice& device, const CreateShaderParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    if (p.desc.bytecode.empty()) {
        ResourceExecuteResult result(CreateShaderResult{});
        result.ProcessFailed("Shader bytecode is empty");
        return result;
    }

    GLenum stage = 0;
    switch (p.desc.stage) {
        case ShaderStage::VERTEX: stage = GL_VERTEX_SHADER; break;
        case ShaderStage::FRAGMENT: stage = GL_FRAGMENT_SHADER; break;
        case ShaderStage::GEOMETRY: stage = GL_GEOMETRY_SHADER; break;
        case ShaderStage::TESS_CONTROL: stage = GL_TESS_CONTROL_SHADER; break;
        case ShaderStage::TESS_EVAL: stage = GL_TESS_EVALUATION_SHADER; break;
        case ShaderStage::COMPUTE: stage = GL_COMPUTE_SHADER; break;
    }

    ShaderGL shader;
    shader.id = glCreateShader(stage);
    if (shader.id == 0) {
        ResourceExecuteResult result(CreateShaderResult{});
        result.ProcessFailed("glCreateShader failed");
        return result;
    }

    const char* src = reinterpret_cast<const char*>(p.desc.bytecode.data());
    GLint len = static_cast<GLint>(p.desc.bytecode.size());
    glShaderSource(shader.id, 1, &src, &len);
    glCompileShader(shader.id);

    GLint status = GL_FALSE;
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen = 0;
        glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        Logging::LoggerFacade::Error(log);
        glGetShaderInfoLog(shader.id, logLen, nullptr, log.data());
        glDeleteShader(shader.id);

        ResourceExecuteResult result(CreateShaderResult{});
        result.ProcessFailed(std::move(log));
        return result;
    }

    ShaderHandle handle = resourceGL.Create<ShaderHandle>(std::move(shader));
    return ResourceExecuteResult(CreateShaderResult{ handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessDestroyShaderImpl(BaseDevice& device, const DestroyShaderParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    bool exists = resourceGL.ContainsId(p.handle.id);
    if (exists) resourceGL.RemoveById(p.handle.id);

    if (!exists) {
        ResourceExecuteResult result(DestroyShaderResult{ p.handle });
        result.ProcessFailed("Shader not found");
        return result;
    }

    return ResourceExecuteResult(DestroyShaderResult{ p.handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCreateSamplerImpl(BaseDevice& device, const CreateSamplerParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    SamplerGL sampler;
    glGenSamplers(1, &sampler.id);
    if (sampler.id == 0) {
        ResourceExecuteResult result(CreateSamplerResult{});
        result.ProcessFailed("glGenSamplers failed");
        return result;
    }

    glSamplerParameteri(sampler.id, GL_TEXTURE_MIN_FILTER, ToGLMip(p.desc.minFilter, p.desc.mipFilter));
    glSamplerParameteri(sampler.id, GL_TEXTURE_MAG_FILTER, ToGL(p.desc.magFilter));
    glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_S, ToGL(p.desc.addressU));
    glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_T, ToGL(p.desc.addressV));
    glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_R, ToGL(p.desc.addressW));
    glSamplerParameterf(sampler.id, GL_TEXTURE_MIN_LOD, p.desc.minLod);
    glSamplerParameterf(sampler.id, GL_TEXTURE_MAX_LOD, p.desc.maxLod);
    glSamplerParameterf(sampler.id, GL_TEXTURE_LOD_BIAS, p.desc.mipLodBias);
    glSamplerParameteri(sampler.id, GL_TEXTURE_COMPARE_MODE,
                        p.desc.compareOp == CompareOp::NEVER ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE);
    if (p.desc.compareOp != CompareOp::NEVER) {
        glSamplerParameteri(sampler.id, GL_TEXTURE_COMPARE_FUNC, ToGL(p.desc.compareOp));
    }
    glSamplerParameteri(sampler.id, GL_TEXTURE_MAX_ANISOTROPY, static_cast<GLint>(p.desc.maxAnisotropy));

    float borderColor[4];
    switch (p.desc.borderColor) {
        case BorderColor::TRANSPARENT_BLACK: borderColor[0]=0; borderColor[1]=0; borderColor[2]=0; borderColor[3]=0; break;
        case BorderColor::OPAQUE_BLACK: borderColor[0]=0; borderColor[1]=0; borderColor[2]=0; borderColor[3]=1; break;
        case BorderColor::OPAQUE_WHITE: borderColor[0]=1; borderColor[1]=1; borderColor[2]=1; borderColor[3]=1; break;
    }
    glSamplerParameterfv(sampler.id, GL_TEXTURE_BORDER_COLOR, borderColor);

    SamplerHandle handle = resourceGL.Create<SamplerHandle>(std::move(sampler));
    return ResourceExecuteResult(CreateSamplerResult{ handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessDestroySamplerImpl(BaseDevice& device, const DestroySamplerParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    bool exists = resourceGL.ContainsId(p.handle.id);
    if (exists) resourceGL.RemoveById(p.handle.id);

    if (!exists) {
        ResourceExecuteResult result(DestroySamplerResult{ p.handle });
        result.ProcessFailed("Sampler not found");
        return result;
    }

    return ResourceExecuteResult(DestroySamplerResult{ p.handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCreateTextureImpl(BaseDevice& device, const CreateTextureParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    std::string errorMsg;
    if (!ValidateTextureDesc(p.desc, errorMsg)) {
        ResourceExecuteResult result(CreateTextureResult{});
        result.ProcessFailed(errorMsg);
        return result;
    }

    TextureGL tex;
    tex.target = DetermineTextureTarget(p.desc);
    tex.width = p.desc.width;
    tex.height = p.desc.height;
    tex.depth = p.desc.depth;
    tex.mipLevels = p.desc.mipLevels;
    tex.arrayLayers = p.desc.arrayLayers;
    tex.format = p.desc.format;
    tex.samples = p.desc.samples;
    tex.is3D = (tex.target == GL_TEXTURE_3D);
    tex.isArray = (tex.target == GL_TEXTURE_2D_ARRAY);
    tex.isCube = (tex.target == GL_TEXTURE_CUBE_MAP);
    tex.isMultisample = (tex.target == GL_TEXTURE_2D_MULTISAMPLE);

    glGenTextures(1, &tex.id);
    if (tex.id == 0) {
        ResourceExecuteResult result(CreateTextureResult{});
        result.ProcessFailed("glGenTextures failed");
        return result;
    }

    GLenum internalFormat = FormatToGLInternalFormat(p.desc.format);
    GLenum target = tex.target;

    glBindTexture(target, tex.id);

    if (tex.isMultisample) {
        glTexStorage2DMultisample(target,
            static_cast<GLsizei>(p.desc.samples),
            internalFormat,
            p.desc.width, p.desc.height,
            GL_TRUE);
    }
    else if (tex.is3D) {
        glTexStorage3D(target, p.desc.mipLevels, internalFormat,
            p.desc.width, p.desc.height, p.desc.depth);
    }
    else if (tex.isArray) {
        glTexStorage3D(target, p.desc.mipLevels, internalFormat,
            p.desc.width, p.desc.height, p.desc.arrayLayers);
    }
    else if (tex.isCube) {
        glTexStorage2D(target, p.desc.mipLevels, internalFormat,
            p.desc.width, p.desc.height);
    }
    else {
        glTexStorage2D(target, p.desc.mipLevels, internalFormat,
            p.desc.width, p.desc.height);
    }

    if (HasFlag(p.desc.usage, TextureUsage::SAMPLED)) {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
            p.desc.mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (tex.is3D) glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }

    glBindTexture(target, 0);

    TextureHandle handle = resourceGL.Create<TextureHandle>(std::move(tex));
    return ResourceExecuteResult(CreateTextureResult{ handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessUpdateTextureImpl(BaseDevice& device, const UpdateTextureParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    auto* tex = resourceGL.GetNativeAs<TextureGL>(p.handle);
    if (!tex) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed("Texture not found");
        return result;
    }

    std::string errorMsg;
    if (!ValidateMipLevel(p.mipLevel, tex->mipLevels, errorMsg)) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed(errorMsg);
        return result;
    }
    if (!ValidateArrayLayer(p.arrayLayer, tex->arrayLayers, errorMsg)) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed(errorMsg);
        return result;
    }
    if (p.region.width == 0 || p.region.height == 0) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed("Empty region");
        return result;
    }
    if (p.data.empty()) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed("Empty data");
        return result;
    }
    if (tex->isMultisample) {
        ResourceExecuteResult result(UpdateTextureResult{ p.handle, 0 });
        result.ProcessFailed("Cannot update multisample texture directly");
        return result;
    }

    GLenum format = FormatToGLFormat(tex->format);
    GLenum type = FormatToGLType(tex->format);
    const void* data = p.data.data();
    GLint mip = static_cast<GLint>(p.mipLevel);

    glBindTexture(tex->target, tex->id);

    if (tex->is3D) {
        glTexSubImage3D(tex->target, mip,
            p.region.x, p.region.y, static_cast<GLint>(p.arrayLayer),
            p.region.width, p.region.height, 1,
            format, type, data);
    }
    else if (tex->isArray) {
        glTexSubImage3D(tex->target, mip,
            p.region.x, p.region.y, static_cast<GLint>(p.arrayLayer),
            p.region.width, p.region.height, 1,
            format, type, data);
    }
    else if (tex->isCube) {
        GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(p.arrayLayer);
        glTexSubImage2D(face, mip,
            p.region.x, p.region.y,
            p.region.width, p.region.height,
            format, type, data);
    }
    else {
        glTexSubImage2D(tex->target, mip,
            p.region.x, p.region.y,
            p.region.width, p.region.height,
            format, type, data);
    }

    glBindTexture(tex->target, 0);

    return ResourceExecuteResult(UpdateTextureResult{ p.handle, p.data.size() });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCopyTextureImpl(BaseDevice& device, const CopyTextureParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    auto* src = resourceGL.GetNativeAs<TextureGL>(p.src);
    auto* dst = resourceGL.GetNativeAs<TextureGL>(p.dst);
    if (!src || !dst) {
        ResourceExecuteResult result(CopyTextureResult{ p.src, p.dst });
        result.ProcessFailed("Texture not found");
        return result;
    }
    if (src->isMultisample || dst->isMultisample) {
        ResourceExecuteResult result(CopyTextureResult{ p.src, p.dst });
        result.ProcessFailed("Copy not supported for multisample textures");
        return result;
    }

    GLuint readFbo = 0, drawFbo = 0;
    glGenFramebuffers(1, &readFbo);
    glGenFramebuffers(1, &drawFbo);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src->id, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->id, 0);

    glBlitFramebuffer(p.srcRegion.x, p.srcRegion.y,
                    p.srcRegion.x + p.srcRegion.width, p.srcRegion.y + p.srcRegion.height,
                    p.dstRegion.x, p.dstRegion.y,
                    p.dstRegion.x + p.dstRegion.width, p.dstRegion.y + p.dstRegion.height,
                    GL_COLOR_BUFFER_BIT,
                    p.filter == FilterMode::NEAREST ? GL_NEAREST : GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &readFbo);
    glDeleteFramebuffers(1, &drawFbo);

    return ResourceExecuteResult(CopyTextureResult{ p.src, p.dst });
}

ResourceExecuteResult ResourceExecutorGL::ProcessDestroyTextureImpl(BaseDevice& device, const DestroyTextureParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    bool exists = resourceGL.ContainsId(p.handle.id);
    if (exists) resourceGL.RemoveById(p.handle.id);

    if (!exists) {
        ResourceExecuteResult result(DestroyTextureResult{ p.handle });
        result.ProcessFailed("Texture not found");
        return result;
    }

    return ResourceExecuteResult(DestroyTextureResult{ p.handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessCreateRenderTargetImpl(BaseDevice& device, const CreateRenderTargetParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    std::string errorMsg;
    if (!ValidateRenderTargetDesc(p.desc, errorMsg)) {
        ResourceExecuteResult result(CreateRenderTargetResult{});
        result.ProcessFailed(errorMsg);
        return result;
    }

    RenderTargetGL rt;
    rt.colors = p.desc.colorAttachments;
    rt.resolveTargets = p.desc.resolveAttachments;
    rt.depth = p.desc.depthStencilAttachment;
    rt.samples = p.desc.samples;
    rt.width = p.desc.width;
    rt.height = p.desc.height;

    for (auto h : rt.colors) {
        auto* t = resourceGL.GetNativeAs<TextureGL>(h);
        if (!t) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Color attachment not found");
            return result;
        }
        if (t->samples != p.desc.samples) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Color attachment sample count mismatch");
            return result;
        }
        if (t->width != p.desc.width || t->height != p.desc.height) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Color attachment size mismatch");
            return result;
        }
    }

    if (p.desc.depthStencilAttachment.id != 0) {
        auto* t = resourceGL.GetNativeAs<TextureGL>(rt.depth);
        if (!t) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Depth attachment not found");
            return result;
        }
        if (t->samples != p.desc.samples) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Depth attachment sample count mismatch");
            return result;
        }
    }

    if (!rt.resolveTargets.empty()) {
        if (p.desc.samples == SampleCount::COUNT_1) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Resolve targets require MSAA render target");
            return result;
        }
        if (rt.resolveTargets.size() != rt.colors.size()) {
            ResourceExecuteResult result(CreateRenderTargetResult{});
            result.ProcessFailed("Resolve targets count must match color attachments");
            return result;
        }
        for (auto h : rt.resolveTargets) {
            auto* t = resourceGL.GetNativeAs<TextureGL>(h);
            if (!t) {
                ResourceExecuteResult result(CreateRenderTargetResult{});
                result.ProcessFailed("Resolve target not found");
                return result;
            }
            if (t->samples != SampleCount::COUNT_1) {
                ResourceExecuteResult result(CreateRenderTargetResult{});
                result.ProcessFailed("Resolve target must be non-MSAA");
                return result;
            }
            if (t->width != p.desc.width || t->height != p.desc.height) {
                ResourceExecuteResult result(CreateRenderTargetResult{});
                result.ProcessFailed("Resolve target size mismatch");
                return result;
            }
        }
    }

    glGenFramebuffers(1, &rt.fbo);
    if (rt.fbo == 0) {
        ResourceExecuteResult result(CreateRenderTargetResult{});
        result.ProcessFailed("glGenFramebuffers failed");
        return result;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo);

    std::vector<GLenum> drawBuffers;
    for (size_t i = 0; i < rt.colors.size(); ++i) {
        auto* tex = resourceGL.GetNativeAs<TextureGL>(rt.colors[i]);
        if (!tex) continue;
        GLenum attachment = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, tex->target, tex->id, 0);
        drawBuffers.push_back(attachment);
    }
    if (!drawBuffers.empty()) {
        glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    }

    if (p.desc.depthStencilAttachment.id != 0) {
        auto* tex = resourceGL.GetNativeAs<TextureGL>(rt.depth);
        if (tex) {
            GLenum attachPoint = (tex->format == Format::FLOAT1 || tex->format == Format::FLOAT2)
                ? GL_DEPTH_ATTACHMENT
                : GL_DEPTH_STENCIL_ATTACHMENT;
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachPoint, tex->target, tex->id, 0);
        }
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &rt.fbo);
        ResourceExecuteResult result(CreateRenderTargetResult{});
        result.ProcessFailed("Framebuffer incomplete: " + std::to_string(status));
        return result;
    }

    RenderTargetHandle handle = resourceGL.Create<RenderTargetHandle>(std::move(rt));
    return ResourceExecuteResult(CreateRenderTargetResult{ handle });
}

ResourceExecuteResult ResourceExecutorGL::ProcessDestroyRenderTargetImpl(BaseDevice& device, const DestroyRenderTargetParams& p) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    bool exists = resourceGL.ContainsId(p.handle.id);
    if (exists) resourceGL.RemoveById(p.handle.id);

    if (!exists) {
        ResourceExecuteResult result(DestroyRenderTargetResult{ p.handle });
        result.ProcessFailed("RenderTarget not found");
        return result;
    }

    return ResourceExecuteResult(DestroyRenderTargetResult{ p.handle });
}

bool ResourceExecutorGL::IsValid() {
    return glfwGetCurrentContext() != nullptr;
}