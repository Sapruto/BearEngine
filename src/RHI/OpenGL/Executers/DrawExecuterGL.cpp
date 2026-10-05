#include "RHI/OpenGL/Executors/DrawExecutorGL.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseWindow.h"

using namespace RHI::Base;

namespace RHI::Executors::DrawExecute {
    bool DrawExecutorGL::CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            errorMsg = std::string("Error ") + op + ": " + std::to_string(err);
            errorCode = err;
            return false;
        }
        return true;
    }

    GLenum DrawExecutorGL::GetGLDrawMode(DrawMode mode) {
        switch (mode) {
            case DrawMode::POINTS: return GL_POINTS;
            case DrawMode::LINES: return GL_LINES;
            case DrawMode::LINE_STRIP: return GL_LINE_STRIP;
            case DrawMode::TRIANGLES: return GL_TRIANGLES;
            case DrawMode::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
            default: return GL_TRIANGLES;
        }
    }

    GLenum DrawExecutorGL::GetGLIndexType(IndexDataType type) {
        switch (type) {
            case IndexDataType::UINT8: return GL_UNSIGNED_BYTE;
            case IndexDataType::UINT16: return GL_UNSIGNED_SHORT;
            case IndexDataType::UINT32: return GL_UNSIGNED_INT;
            default: return GL_UNSIGNED_INT;
        }
    }

    DrawExecuteResult DrawExecutorGL::ProcessDrawElementsImpl(RHI::Base::BaseDevice& device, const DrawElementsParam& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;
        unsigned int drawnCount = 0;
        unsigned int indexCount = 0;

        glBindVertexArray(params.vao);
        success = CheckGLError("glBindVertexArray", errorMsg, errorCode);

        if (params.shaderProgram != 0) {
            glUseProgram(params.shaderProgram);
        }

        if (success) {
            GLenum mode = GetGLDrawMode(params.mode);
            GLenum indexType = GetGLIndexType(params.indexDataType);

            std::visit([&](const auto& d) {
                using T = std::decay_t<decltype(d)>;

                if constexpr (std::is_same_v<T, DrawElementsParam::Plain>) {
                    glDrawElementsBaseVertex(
                        mode, d.indexCount, indexType,
                        (const void*)(uintptr_t)d.indexOffset, params.baseVertex);
                    drawnCount = d.indexCount;
                    indexCount = d.indexCount;
                }
                else if constexpr (std::is_same_v<T, DrawElementsParam::Range>) {
                    glDrawRangeElementsBaseVertex(
                        mode, d.minIndex, d.maxIndex, d.indexCount, indexType,
                        (const void*)(uintptr_t)d.indexOffset, params.baseVertex);
                    drawnCount = d.indexCount;
                    indexCount = d.indexCount;
                }
                else if constexpr (std::is_same_v<T, DrawElementsParam::Instanced>) {
                    glDrawElementsInstancedBaseVertexBaseInstance(
                        mode, d.indexCount, indexType,
                        (const void*)(uintptr_t)d.indexOffset, d.instanceCount,
                        params.baseVertex, d.baseInstance);
                    drawnCount = d.indexCount * d.instanceCount;
                    indexCount = d.indexCount;
                }
                else if constexpr (std::is_same_v<T, DrawElementsParam::RangeInstanced>) {
                    glDrawElementsInstancedBaseVertexBaseInstance(
                        mode, d.indexCount, indexType,
                        (const void*)(uintptr_t)d.indexOffset, d.instanceCount,
                        params.baseVertex, d.baseInstance);
                    drawnCount = d.indexCount * d.instanceCount;
                    indexCount = d.indexCount;
                }
            }, params.data);

            success = CheckGLError("glDrawElements*", errorMsg, errorCode);
        }

        DrawElementsResult result;
        result.vaoID = params.vao;
        result.indexCount = indexCount;
        result.drawnCount = drawnCount;
        result.success = success;
        result.errorMessage = errorMsg;
        return DrawExecuteResult(result);
    }

    DrawExecuteResult DrawExecutorGL::ProcessDrawArraysImpl(RHI::Base::BaseDevice& device, const DrawArrays& params) {
        std::string errorMsg;
        GLenum errorCode = GL_NO_ERROR;
        bool success = true;

        glBindVertexArray(params.vao);
        success = CheckGLError("glBindVertexArray", errorMsg, errorCode);

        if (params.shaderProgram != 0) {
            glUseProgram(params.shaderProgram);
        }

        if (success) {
            glDrawArrays(GetGLDrawMode(params.mode), params.first, params.count);
            success = CheckGLError("glDrawArrays", errorMsg, errorCode);
        }

        DrawArraysResult result;
        result.vaoID = params.vao;
        result.count = params.count;
        result.drawnCount = success ? params.count : 0;
        result.success = success;
        result.errorMessage = errorMsg;
        return DrawExecuteResult(result);
    }

    bool DrawExecutorGL::IsValid() {
        return glfwGetCurrentContext() != nullptr;
    }
}