#pragma once

#include "RHI/Base/Executors/DrawExecutor.h"
#include "RHI/Base/Models/DrawExecutorParams.h"
#include "RHI/Base/Models/DrawExecutorResult.h"

#include "glad/glad.h"

#include <string>

namespace RHI::Executors::DrawExecute {
    class DrawExecutorGL final : public DrawExecutor<DrawExecutorGL> {
    private:
        bool CheckGLError(const char* op, std::string& errorMsg, GLenum& errorCode);
        GLenum GetGLDrawMode(DrawMode mode);
        GLenum GetGLIndexType(IndexDataType type);

    public:
        DrawExecuteResult ProcessDrawElementsImpl(RHI::Base::BaseDevice& device, const DrawElementsParam& params);
        DrawExecuteResult ProcessDrawArraysImpl(RHI::Base::BaseDevice& device, const DrawArrays& params);

        bool IsValid() override;
    };
}