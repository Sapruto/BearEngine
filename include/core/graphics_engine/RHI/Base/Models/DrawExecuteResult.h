#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include "RHI/Base/Executers/BaseExecuterRHI.h"

namespace DrawExecute {
    struct DrawElementsResult {
        unsigned int vaoID;
        unsigned int indexCount;
        unsigned int drawnCount;
        DrawMode mode;
        IndexDataType indexDataType;
        bool isInstanced;
        unsigned int instanceCount;
        bool success;
        std::string errorMessage;
    };

    struct DrawArraysResult {
        unsigned int vaoID;
        unsigned int first;
        unsigned int count;
        unsigned int drawnCount;
        DrawMode mode;
        bool success;
        std::string errorMessage;
    };

    struct DrawExecuteResult : public BaseProcessResult {
        std::variant<
            DrawElementsResult,
            DrawArraysResult
        > data;

        DrawExecuteResult() = default;
        DrawExecuteResult(const DrawElementsResult& r) : data(r) {}
        DrawExecuteResult(const DrawArraysResult& r)   : data(r) {}
    };
}