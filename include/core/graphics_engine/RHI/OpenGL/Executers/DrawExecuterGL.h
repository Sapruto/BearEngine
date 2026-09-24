#pragma once

#include "RHI/Base/Executers/DrawExecuter.h"
#include "RHI/Base/Models/DrawExecuteParams.h"
#include "RHI/Base/Models/DrawExecuteResult.h"

#include "glad/glad.h"

#include <string>

namespace DrawExecute {

    class DrawExecuterGL final : public DrawExecuter<DrawExecuterGL> {
    private:
        bool CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode);
        GLenum GetGLDrawMode(DrawMode mode);
        GLenum GetGLIndexType(IndexDataType type);

    public:
        DrawExecuteResult ProcessDrawElementsImpl(BaseDevice& device, const DrawElementsParam& params);
        DrawExecuteResult ProcessDrawArraysImpl(BaseDevice& device, const DrawArrays& params);

        bool IsValid();
    };
}