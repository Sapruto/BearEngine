#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"
#include "RHI/Base/Models/DrawExecutorParams.h"
#include "RHI/Base/Models/DrawExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::DrawExecute {
    template<typename T>
    concept HasDrawExecutorImpl = requires(T* t, RHI::Base::BaseDevice& device,
                                           const DrawElementsParam& drawElements,
                                           const DrawArrays& drawArrays) {
        { t->ProcessDrawElementsImpl(device, drawElements) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessDrawArraysImpl(device, drawArrays) } -> std::same_as<DrawExecuteResult>;
        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename DrawExecutorImpl>
    class DrawExecutor : public BaseExecutorRHI<DrawExecutorImpl, DrawParams, DrawExecuteResult> {
        using Base = BaseExecutorRHI<DrawExecutorImpl, DrawParams, DrawExecuteResult>;

    public:
        DrawExecutor() {
            static_assert(HasDrawExecutorImpl<DrawExecutorImpl>,
                        "DrawExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::Draw));
            this->type = ExecutorTypeRHI::DrawExecutor;
        }

        DrawExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const DrawParams& param) {
            auto* self = static_cast<DrawExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> DrawExecuteResult {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, DrawElementsParam>) {
                    return self->ProcessDrawElementsImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, DrawArrays>) {
                    return self->ProcessDrawArraysImpl(device, p);
                }
                else {
                    DrawExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown draw param type";
                    return result;
                }
            }, param);
        }

        DrawExecuteResult ProcessDrawElements(RHI::Base::BaseDevice& device, const DrawElementsParam& params) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessDrawElementsImpl(device, params);
        }

        DrawExecuteResult ProcessDrawArrays(RHI::Base::BaseDevice& device, const DrawArrays& params) {
            return static_cast<DrawExecutorImpl*>(this)->ProcessDrawArraysImpl(device, params);
        }
    };
}