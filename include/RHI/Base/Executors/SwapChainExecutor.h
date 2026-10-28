#pragma once

#include <concepts>

#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Contracts/SwapChainExecutorResult.h"
#include "RHI/Base/Executors/BaseExecutorRHI.h"
#include "RHI/Base/Executors/BaseExecutorLayers.h"

namespace RHI::Executors::SwapChainExecutor {
    template<typename T>
    concept HasSwapChainExecutorImpl = requires (T* t, RHI::Base::BaseDevice& device,
                                                const SwapChainCreateParams& create,
                                                const SwapChainResizeParams& resize,
                                                const SwapChainPresentParams& present,
                                                const SwapChainDestroyParams& destroy) {
        { t->ProcessCreateSwapChainImpl(device, create) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessResizeSwapChainImpl(device, resize) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessPresentSwapChainImpl(device, present) } -> std::same_as<SwapChainExecuteResult>;
        { t->ProcessDestroySwapChainImpl(device, destroy) } -> std::same_as<SwapChainExecuteResult>;

        { t->IsValid() } -> std::same_as<bool>;
    };

    template<typename SwapChainExecutorImpl>
    class SwapChainExecutor : public BaseExecutorRHI<SwapChainExecutorImpl, SwapChainParams, SwapChainExecuteResult> {
    public:
        SwapChainExecutor() {
            static_assert(HasSwapChainExecutorImpl<SwapChainExecutorImpl>,
                        "SwapChainExecutorImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecutorLayers::SwapChain));
            this->type = ExecutorTypeRHI::SwapChainExecutor;
        }

        SwapChainExecuteResult ProcessCurrentParam(RHI::Base::BaseDevice& device, const SwapChainParams& param) {
            auto* self = static_cast<SwapChainExecutorImpl*>(this);
            return std::visit([&](const auto& p) -> SwapChainExecuteResult {
                using T = std::decay_t<decltype(p)>;

                if constexpr (std::is_same_v<T, SwapChainCreateParams>) {
                    return self->ProcessCreateSwapChain(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainResizeParams>) {
                    return self->ProcessResizeSwapChain(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainPresentParams>) {
                    return self->ProcessPresentSwapChain(device, p);
                }
                else if constexpr (std::is_same_v<T, SwapChainDestroyParams>) {
                    return self->ProcessDestroySwapChain(device, p);
                }
                else {
                    SwapChainExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown swapchain param type";
                    return result;
                }
            }, param);
        }

        SwapChainExecuteResult ProcessCreateSwapChain(RHI::Base::BaseDevice& device, const SwapChainCreateParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessCreateSwapChainImpl(device, p);
        }

        SwapChainExecuteResult ProcessResizeSwapChain(RHI::Base::BaseDevice& device, const SwapChainResizeParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessResizeSwapChainImpl(device, p);
        }

        SwapChainExecuteResult ProcessPresentSwapChain(RHI::Base::BaseDevice& device, const SwapChainPresentParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessPresentSwapChainImpl(device, p);
        }

        SwapChainExecuteResult ProcessDestroySwapChain(RHI::Base::BaseDevice& device, const SwapChainDestroyParams& p) {
            return static_cast<SwapChainExecutorImpl*>(this)->ProcessDestroySwapChainImpl(device, p);
        }
    };
}