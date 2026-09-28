#pragma once

#include <concepts>
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
        { t->ProcessDrawArraysImpl(device, drawArrays) }     -> std::same_as<DrawExecuteResult>;
        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename DrawExecuterImpl>
    class DrawExecuter : public BaseExecuterRHI<DrawExecuterImpl, DrawParams, DrawExecuteResult> {
    private:
        DrawExecuterImpl* impl;

    public:
        DrawExecuter() : impl(static_cast<DrawExecuterImpl*>(this)) {
            this->SetLayer(static_cast<unsigned int>(BaseExecuterLayers::Draw));
            this->type = ExecuterTypeRHI::DrawExecuter;
        }

        DrawExecuteResult ProcessDrawElements(BaseDevice& device, const DrawElementsParam& params) {
            return impl->ProcessDrawElementsImpl(device, params);
        }

        DrawExecuteResult ProcessDrawArrays(BaseDevice& device, const DrawArrays& params) {
            return impl->ProcessDrawArraysImpl(device, params);
        }

        DrawExecuteResult ProcessCurrentParam(BaseDevice& device, const DrawParams& param) {
            return std::visit([&](const auto& p) -> DrawExecuteResult {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, DrawElementsParam>) {
                    return ProcessDrawElements(device, p);
                }
                else if constexpr (std::is_same_v<T, DrawArrays>) {
                    return ProcessDrawArrays(device, p);
                }
                else {
                    DrawExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown draw param type";
                    return result;
                }
            }, param);
        }

        void ProcessParamsImpl(BaseDevice& device) {
            if (!impl->IsValid()) return;

            std::vector<DrawExecuteResult> results;
            results.reserve(this->params.size());

            for (size_t i = 0; i < this->params.size(); ++i) {
                const auto& param = this->params[i];

                DrawExecuteResult result = ProcessCurrentParam(device, param);
                result.paramID = static_cast<int>(i);

                results.push_back(std::move(result));
            }

            this->NotifySubscribers(results);
            this->ClearParams();
        }
    };
}