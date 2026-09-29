#pragma once

#include <concepts>
#include <variant>
#include <type_traits>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"
#include "RHI/Base/Executers/BaseExecuterLayers.h"

namespace SwapChainExecute {
    template<typename T>
    concept HasSwapChainExecuterImpl = requires(T* t, BaseDevice& device,
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

    template<typename SwapChainExecuterImpl>
    class SwapChainExecuter : public BaseExecuterRHI<SwapChainExecuterImpl, SwapChainParams, SwapChainExecuteResult> {
        using Base = BaseExecuterRHI<SwapChainExecuterImpl, SwapChainParams, SwapChainExecuteResult>;

    public:
        SwapChainExecuter() {
            static_assert(HasSwapChainExecuterImpl<SwapChainExecuterImpl>,
                        "HasSwapChainExecuterImpl must implement all Process*Impl methods and IsValid()");

            this->SetLayer(static_cast<unsigned int>(BaseExecuterLayers::SwapChain));
            this->type = ExecuterTypeRHI::SwapChainExecuter;
        }

        SwapChainExecuteResult ProcessCurrentParam(BaseDevice& device, const SwapChainParams& param) {
            auto* self = static_cast<SwapChainExecuterImpl*>(this);
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

        SwapChainExecuteResult ProcessSwapChainCreate(BaseDevice& device, const SwapChainCreateParams& p) {
            return static_cast<SwapChainExecuterImpl*>(this)->ProcessSwapChainCreateImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainResize(BaseDevice& device, const SwapChainResizeParams& p) {
            return static_cast<SwapChainExecuterImpl*>(this)->ProcessSwapChainResizeImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainPresent(BaseDevice& device, const SwapChainPresentParams& p) {
            return static_cast<SwapChainExecuterImpl*>(this)->ProcessSwapChainPresentImpl(device, p);
        }

        SwapChainExecuteResult ProcessSwapChainDestroy(BaseDevice& device, const SwapChainDestroyParams& p) {
            return static_cast<SwapChainExecuterImpl*>(this)->ProcessSwapChainDestroyImpl(device, p);
        }
    };
}