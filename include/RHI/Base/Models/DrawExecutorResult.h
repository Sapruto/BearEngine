#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include "RHI/Base/Executors/BaseExecutorRHI.h"

namespace RHI::Executors::DrawExecute {
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

        DrawElementsResult() = default;
        DrawElementsResult(unsigned int vaoID, unsigned int indexCount, unsigned int drawnCount,
                           DrawMode mode, IndexDataType indexDataType, bool isInstanced,
                           unsigned int instanceCount, bool success,
                           const std::string& errorMessage = "")
            : vaoID(vaoID), indexCount(indexCount), drawnCount(drawnCount),
              mode(mode), indexDataType(indexDataType), isInstanced(isInstanced),
              instanceCount(instanceCount), success(success), errorMessage(errorMessage) {}
    };

    struct DrawArraysResult {
        unsigned int vaoID;
        unsigned int first;
        unsigned int count;
        unsigned int drawnCount;
        DrawMode mode;
        bool success;
        std::string errorMessage;

        DrawArraysResult() = default;
        DrawArraysResult(unsigned int vaoID, unsigned int first, unsigned int count,
                         unsigned int drawnCount, DrawMode mode, bool success,
                         const std::string& errorMessage = "")
            : vaoID(vaoID), first(first), count(count), drawnCount(drawnCount),
              mode(mode), success(success), errorMessage(errorMessage) {}
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