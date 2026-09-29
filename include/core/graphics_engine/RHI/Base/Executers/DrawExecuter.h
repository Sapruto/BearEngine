#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/DrawExecuteParams.h"
#include "RHI/Base/Models/DrawExecuteResult.h"
#include "RHI/Base/Executers/BaseExecuterLayers.h"

namespace DrawExecute {
    template<typename T>
    concept HasDrawExecuterImpl = requires(T* t, BaseDevice& device,
                                           const DrawElementsParam& drawElements,
                                           const DrawArrays& drawArrays) {
        { t->ProcessDrawElementsImpl(device, drawElements) } -> std::same_as<DrawExecuteResult>;
        { t->ProcessDrawArraysImpl(device, drawArrays) } -> std::same_as<DrawExecuteResult>;
        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename DrawExecuterImpl>
    class DrawExecuter : public BaseExecuterRHI<DrawExecuterImpl, DrawParams, DrawExecuteResult> {
        using Base = BaseExecuterRHI<DrawExecuterImpl, DrawParams, DrawExecuteResult>;

    public:
        DrawExecuter() {
            static_assert(HasDrawExecuterImpl<DrawExecuterImpl>,
                        "DrawExecuterImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecuterLayers::Draw));
            this->type = ExecuterTypeRHI::DrawExecuter;
        }

        DrawExecuteResult ProcessCurrentParam(BaseDevice& device, const DrawParams& param) {
            auto* self = static_cast<DrawExecuterImpl*>(this);
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

        DrawExecuteResult ProcessDrawElements(BaseDevice& device, const DrawElementsParam& params) {
            return static_cast<DrawExecuterImpl*>(this)->ProcessDrawElementsImpl(device, params);
        }

        DrawExecuteResult ProcessDrawArrays(BaseDevice& device, const DrawArrays& params) {
            return static_cast<DrawExecuterImpl*>(this)->ProcessDrawArraysImpl(device, params);
        }
    };
}