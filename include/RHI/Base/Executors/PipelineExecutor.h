#pragma once

#include <concepts>

#include "RHI/Base/Contracts/PipelineExecutorParams.h"
#include "RHI/Base/Contracts/PipelineExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::PipelineExecutor {
    template<typename T>
    concept HasPipelineExecutorImpl = requires(T* t, RHI::Base::BaseDevice& device,
                                            const CreatePipelineParams& create,
                                            const DestroyPipelineParams& destroy) {
        { t->ProcessCreatePipelineImpl(device, create) } -> std::same_as<PipelineExecuteResult>;
        { t->ProcessDestroyPipelineImpl(device, destroy) } -> std::same_as<PipelineExecuteResult>;

        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename PipelineExecutorImpl>
    class PipelineExecutor : public BaseExecutorRHI<PipelineExecutorImpl, PipelineParams, PipelineExecuteResult> {
    public:
        PipelineExecutor() {
            static_assert(HasPipelineExecutorImpl<PipelineExecutorImpl>,
                        "PipelineExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::Pipeline));
            this->type = ExecutorTypeRHI::PipelineExecutor;
        }

        PipelineExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const PipelineParams& param) {
            auto* self = static_cast<PipelineExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> PipelineExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, CreatePipelineParams>) {
                    return self->ProcessCreatePipeline(device, p);
                }
                else if constexpr (std::is_same_v<T, DestroyPipelineParams>) {
                    return self->ProcessDestroyPipeline(device, p);
                }
                else {
                    PipelineExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown pipeline param type";
                    return result;
                }
            }, param);
        }

        PipelineExecuteResult ProcessCreatePipeline(RHI::Base::BaseDevice& device, const CreatePipelineParams& p) {
            return static_cast<PipelineExecutorImpl*>(this)->ProcessCreatePipelineImpl(device, p);
        }

        PipelineExecuteResult ProcessDestroyPipeline(RHI::Base::BaseDevice& device, const DestroyPipelineParams& p) {
            return static_cast<PipelineExecutorImpl*>(this)->ProcessDestroyPipelineImpl(device, p);
        }
    };
}