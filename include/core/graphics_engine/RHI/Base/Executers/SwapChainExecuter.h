#pragma once

#include <concepts>
#include "RHI/Base/Executers/BaseExecuterRHI.h"
#include "RHI/Base/Executers/ExecuterTypeRHI.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

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
    requires(HasSwapChainExecuterImpl<SwapChainExecuterImpl>)
    class SwapChainExecuter : public BaseExecuterRHI<SwapChainExecuterImpl, SwapChainParams, SwapChainExecuteResult> {
    private:
        SwapChainExecuterImpl* impl;

    protected:
        SwapChainExecuteResult ProcessSwapChainCreate(BaseDevice& device, const SwapChainCreateParams& params) {
            return impl->ProcessSwapChainCreateImpl(device, params);
        }
        
        SwapChainExecuteResult ProcessSwapChainResize(BaseDevice& device, const SwapChainResizeParams& params) {
            return impl->ProcessSwapChainResizeImpl(device, params);
        }
        
        SwapChainExecuteResult ProcessSwapChainPresent(BaseDevice& device, const SwapChainPresentParams& params) {
            return impl->ProcessSwapChainPresentImpl(device, params);
        }
        
        SwapChainExecuteResult ProcessSwapChainDestroy(BaseDevice& device, const SwapChainDestroyParams& params) {
            return impl->ProcessSwapChainDestroyImpl(device, params);
        }
        
        SwapChainExecuteResult ProcessCurrentParam(BaseDevice& device, const SwapChainParams& param) {
            ParamType type = std::visit([](const auto& p) -> ParamType {
                return p.GetType();
            }, param);
            
            switch (type) {
                case ParamType::CREATE:
                    return ProcessSwapChainCreate(device, std::get<SwapChainCreateParams>(param));
                    
                case ParamType::RESIZE:
                    return ProcessSwapChainResize(device, std::get<SwapChainResizeParams>(param));
                    
                case ParamType::PRESENT:
                    return ProcessSwapChainPresent(device, std::get<SwapChainPresentParams>(param));
                    
                case ParamType::DESTROY:
                    return ProcessSwapChainDestroy(device, std::get<SwapChainDestroyParams>(param));
                    
                default:
                    SwapChainExecuteResult result;
                    result.success = false;
                    result.errorMessage = "Unknown parameter type";
                    return result;
            }
        }
        
    public:
        SwapChainExecuter() : impl(static_cast<SwapChainExecuterImpl*>(this)) {
            this->type = ExecuterTypeRHI::SwapChainExecuter;
        }
        
        void ProcessParamsImpl(BaseDevice& device) {
            if (!impl->IsValid()) return;

            std::vector<SwapChainExecuteResult> results;
            results.reserve(this->params.size());
            
            for (size_t i = 0; i < this->params.size(); ++i) {
                const auto& param = this->params[i];
                
                SwapChainExecuteResult result = ProcessCurrentParam(device, param);
                result.paramID = static_cast<int>(i);
                
                results.push_back(std::move(result));
            }
            
            this->NotifySubscribers(results);
            this->ClearParams();
        }
    };
}