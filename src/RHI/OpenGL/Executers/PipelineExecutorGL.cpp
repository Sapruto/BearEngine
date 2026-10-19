#include "RHI/OpenGL/Executors/PipelineExecutorGL.h"

#include "Logger/Logger.h"
#include "RHI/Base/Enums.h"
#include "RHI/Base/Contracts/PipelineExecutorParams.h"
#include "RHI/Base/Contracts/PipelineExecutorResult.h"
#include "RHI/Base/Handles.h"
#include "RHI/OpenGL/Core/ResourceGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Utilities.h"
#include "RHI/OpenGL/HandlesGL.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"

using namespace RHI;
using namespace RHI::Handles;
using namespace RHI::OpenGL;
using namespace RHI::OpenGL::Utilities;
using namespace RHI::Executors::PipelineExecutor;

void PipelineExecutorGL::SetVertexAttributes(GLuint& vao, const std::vector<VertexAttributeDesc>& attributes, const std::vector<VertexBindingDesc>& bindings) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    try {
        for (const auto& attr : attributes) {
            GLint components = FormatComponents(attr.format);
            GLenum type = FormatComponentType(attr.format);
            GLboolean normalized = FormatIsNormalized(attr.format);

            glVertexAttribFormat(attr.location, components, type, normalized, attr.offset);
            glVertexAttribBinding(attr.location, attr.binding);
            glEnableVertexAttribArray(attr.location);
        }

        for (const auto& bind : bindings) {
            if (bind.perInstance) {
                glVertexBindingDivisor(bind.binding, bind.instanceDivisor);
            }
        }

        glBindVertexArray(0);
    }
    catch(const std::exception& e) {
        glDeleteVertexArrays(1, &vao);
        throw;
    }
}

void PipelineExecutorGL::SetRasterizer(CullMode cullMode, FrontFace frontFace, FillMode fillMode,
                    float depthBias, float slopeScaledDepthBias, bool depthClampEnable) {
    if (cullMode == CullMode::NONE) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
        glCullFace(
            cullMode == CullMode::FRONT ? GL_FRONT :
            cullMode == CullMode::BACK ? GL_BACK  : GL_FRONT_AND_BACK
        );
    }

    glFrontFace(frontFace == FrontFace::CLOCKWISE ? GL_CW : GL_CCW);

    glPolygonMode(GL_FRONT_AND_BACK,
        fillMode == FillMode::SOLID     ? GL_FILL :
        fillMode == FillMode::WIREFRAME ? GL_LINE : GL_POINT);

    if (depthBias != 0.0f || slopeScaledDepthBias != 0.0f) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(slopeScaledDepthBias, depthBias);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    if (depthClampEnable) {
        glEnable(GL_DEPTH_CLAMP);
    }
    else {
        glDisable(GL_DEPTH_CLAMP);
    }
}

void PipelineExecutorGL::SetDepth(bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp) {
    if (depthTestEnable) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(ToGL(depthCompareOp));
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glDepthMask(depthWriteEnable ? GL_TRUE : GL_FALSE);
}

void PipelineExecutorGL::SetStencil(bool stencilTestEnable, uint32_t stencilReadMask, uint32_t stencilWriteMask,
                StencilFaceState stencilFront, StencilFaceState stencilBack) {
    if (stencilTestEnable) {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, stencilReadMask);
        glStencilMask(stencilWriteMask);
        glStencilOpSeparate(GL_FRONT,
            ToGL(stencilFront.failOp),
            ToGL(stencilFront.depthFailOp),
            ToGL(stencilFront.passOp));
        glStencilOpSeparate(GL_BACK,
            ToGL(stencilBack.failOp),
            ToGL(stencilBack.depthFailOp),
            ToGL(stencilBack.passOp));
    }
    else {
        glDisable(GL_STENCIL_TEST);
    }
}

void PipelineExecutorGL::SetBlend(const std::vector<BlendAttachmentState>& blendAttachments) {
    if (blendAttachments.empty()) return;

    for (uint32_t i = 0; i < blendAttachments.size(); i++) {
        const auto& b = blendAttachments[i];

        if (b.blendEnable) {
            glEnablei(GL_BLEND, i);
            glBlendFuncSeparatei(i,
                ToGL(b.srcColorBlendFactor), ToGL(b.dstColorBlendFactor),
                ToGL(b.srcAlphaBlendFactor), ToGL(b.dstAlphaBlendFactor));
            glBlendEquationSeparatei(i,
                ToGL(b.colorBlendOp),
                ToGL(b.alphaBlendOp));
        }
        else {
            glDisablei(GL_BLEND, i);
        }

        glColorMaski(i,
            (b.writeMask & ColorWriteMask::R) != ColorWriteMask::NONE,
            (b.writeMask & ColorWriteMask::G) != ColorWriteMask::NONE,
            (b.writeMask & ColorWriteMask::B) != ColorWriteMask::NONE,
            (b.writeMask & ColorWriteMask::A) != ColorWriteMask::NONE);
    }
}

void PipelineExecutorGL::SetMultisample(bool alphaToCoverage, bool alphaToOne) {
    if (alphaToCoverage) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    if (alphaToOne) {
        glEnable(GL_SAMPLE_ALPHA_TO_ONE);
    }
    else {
        glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    }
}

void PipelineExecutorGL::ApplyPipelineByHandle(RHI::Base::BaseDevice& device, RHI::Handles::PipelineHandle pipelineHandle) {
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    PipelineGL* pipeline = resourceGL.GetNativeAs<PipelineGL, PipelineHandle>(pipelineHandle);
    if (!pipeline) {
        return;
    }

    const auto& s = pipeline->state;

    SetRasterizer(s.cullMode, s.frontFace, s.fillMode,
                s.depthBias, s.slopeScaledDepthBias, s.depthClampEnable);
    SetDepth(s.depthTestEnable, s.depthWriteEnable, s.depthCompareOp);
    SetStencil(s.stencilTestEnable, s.stencilReadMask, s.stencilWriteMask,
            s.stencilFront, s.stencilBack);
    SetBlend(s.blendAttachments);
    SetMultisample(s.alphaToCoverage, s.alphaToOne);

    glUseProgram(pipeline->program);
    glBindVertexArray(pipeline->vao);
}

PipelineExecuteResult PipelineExecutorGL::ProcessCreatePipelineImpl(RHI::Base::BaseDevice& device, const CreatePipelineParams& params) {
    //!The custom method does not
    //!apply any states because OpenGL only has a global state,
    //!so all the parameters we save will be applied directly in the DrawExecutor.
    
    PipelineExecuteResult result;
    
    uint64_t hash = params.state.Hash();
    auto it = pipelineCache.find(hash);
    if (it != pipelineCache.end()) {
        result.data = CreatePipelineResult(it->second);
        return result;
    }
    
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());
    
    GLuint program = glCreateProgram();

    std::vector<GLuint> attached;
    for (const auto& desc : params.state.stages) {
        ShaderGL* shader = resourceGL.GetNativeAs<ShaderGL, ShaderHandle>(desc.shaderHandler);
        if (!shader) {
            glDeleteProgram(program);
            result.ProcessFailed("Shader not found");
            Logging::LoggerFacade::Error(result.errorMessage + "\n");
            return result;
        }
        glAttachShader(program, shader->id);
        attached.push_back(shader->id);
    }

    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen > 0 ? logLen : 1, '\0');
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        glDeleteProgram(program);
        result.ProcessFailed("Link failed: " + log);
        Logging::LoggerFacade::Error(result.errorMessage + "\n");
        return result;
    }

    for (const auto& attach : attached) {
        glDetachShader(program, attach);
    }

    GLuint vao;
    try {
        SetVertexAttributes(vao, params.state.vertexAttributes, params.state.vertexBindings);
    }
    catch (const std::exception& e) {
        glDeleteProgram(program);
        result.ProcessFailed(e.what());
        Logging::LoggerFacade::Error(result.errorMessage + "\n");
        return result;
    }

    for (const auto& ub : params.state.uniformBindings) {
        if (ub.name.empty()) continue;

        GLuint blockIndex = glGetUniformBlockIndex(program, ub.name.c_str());
        if (blockIndex == GL_INVALID_INDEX) {
            glDeleteProgram(program);
            result.ProcessFailed("Uniform block not found: " + ub.name);
            Logging::LoggerFacade::Error(result.errorMessage + "\n");
            return result;
        }
        glUniformBlockBinding(program, blockIndex, ub.binding);
    }

    PipelineGL pipe;
    pipe.program = program;
    pipe.vao = vao;
    pipe.state = params.state;

    PipelineHandle handle = resourceGL.Create<PipelineHandle>(std::move(pipe));
    pipelineCache[hash] = handle;
    result.data = CreatePipelineResult(handle);
    return result;
}

PipelineExecuteResult PipelineExecutorGL::ProcessDestroyPipelineImpl(RHI::Base::BaseDevice& device, const DestroyPipelineParams& params)
{
    DeviceGL& deviceGL = static_cast<DeviceGL&>(device);
    ResourceGL& resourceGL = static_cast<ResourceGL&>(*deviceGL.GetResource());

    for (auto it = pipelineCache.begin(); it != pipelineCache.end(); ) {
        if (it->second == params.handle) {
            it = pipelineCache.erase(it);
        }
        else {
            it++;
        }
    }

    bool exists = resourceGL.ContainsId(params.handle.id);
    if (exists) resourceGL.RemoveById(params.handle.id);

    PipelineExecuteResult result;
    result.success = exists;
    result.errorMessage = exists ? "" : "Pipeline not found";
    result.data = DestroyPipelineResult();
    return result;
}

bool PipelineExecutorGL::IsValid() {
    return glfwGetCurrentContext() != nullptr;
}