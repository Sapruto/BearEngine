#pragma once

#include <variant>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/GeometryExecuteParams.h"
#include "RHI/Base/Models/GeometryExecuteResult.h"

namespace GeometryExecute {
    template<typename GeometryExecuterImpl>
    class GeometryExecuter : public BaseExecuterRHI<GeometryExecuterImpl, GeometryParams, GeometryExecuteResult> {
    public:
        GeometryExecuter() {
            type = ExecuterTypeRHI::GeometryExecuter;
        }
    };
};