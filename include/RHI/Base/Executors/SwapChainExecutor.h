#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"
#include "RHI/Base/Models/SwapChainExecutorParams.h"
#include "RHI/Base/Models/SwapChainExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::SwapChainExecute {
    template<typename T>
    concept HasSwapChainExecutorImpl = requires(T* t, RHI::Base::BaseDevice& device,
                                                const SwapChainCreateParams& create,
                                                const SwapChainResizeParams& resize,
                                                const SwapChainPresentParams& present,
                                                const SwapChainDestroyParams& destroy) {
        { t->ProcessSwapChainCreateImpl(device, create) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessSwapChainResizeImpl(device, resize) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessSwapChainPresentImpl(device, present) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessSwapChainDestroyImpl(device, destroy) } -> std::same_as<SwapChainExecuteResult>;
        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename SwapChainExecutorImpl>
    class SwapChainExecutor : public BaseExecutorRHI<SwapChainExecutorImpl, SwapChainParams, SwapChainExecuteResult> {
        using Base = BaseExecutorRHI<SwapChainExecutorImpl, SwapChainParams, SwapChainExecuteResult>;

    public:
        SwapChainExecutor() {
            static_assert(HasSwapChainExecutorImpl<SwapChainExecutorImpl>,
                        "HasSwapChainExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::SwapChain));
            this->type = ExecutorTypeRHI::SwapChainExecutor;
        }

        SwapChainExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const SwapChainParams& param) {
            auto* self = static_cast<SwapChainExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> SwapChainExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, SwapChainCreateParams>) {
                    return self->ProcessSwapChainCreateImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainResizeParams>) {
                    return self->ProcessSwapChainResizeImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainPresentParams>) {
                    return self->ProcessSwapChainPresentImpl(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainDestroyParams>) {
                    return self->ProcessSwapChainDestroyImpl(device, p);
                }
                else {
                    SwapChainExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown swapchain param type";
                    return result;
                }
            }, param);
        }

        SwapChainExecuteResult ProcessSwapChainCreate(RHI::Base::BaseDevice& device, const SwapChainCreateParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessSwapChainCreateImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainResize(RHI::Base::BaseDevice& device, const SwapChainResizeParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessSwapChainResizeImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainPresent(RHI::Base::BaseDevice& device, const SwapChainPresentParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessSwapChainPresentImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainDestroy(RHI::Base::BaseDevice& device, const SwapChainDestroyParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessSwapChainDestroyImpl(device, p);
        }
    };
}