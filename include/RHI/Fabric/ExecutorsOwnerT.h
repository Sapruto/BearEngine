#pragma once

#include <memory>
#include <vector>

#include "RHI/Fabric/IExecutorsOwner.h"

namespace RHI::FabricRHI {
    using RHI::Executors::IExecutorRHI;
    using RHI::Executors::ExecutorTypeRHI;

    template<typename Backend>
    class ExecutorsOwnerT : public IExecutorsOwner {
    private:
        std::unique_ptr<typename Backend::ResourceExecutor> resourceExec;
        std::unique_ptr<typename Backend::PipelineExecutor> pipelineExec;
        std::unique_ptr<typename Backend::DrawExecutor> drawExec;
        std::unique_ptr<typename Backend::SwapChainExecutor> swapChainExec;

        std::vector<IExecutorRHI*> raw;

    public:
        ExecutorsOwnerT() {}
        ~ExecutorsOwnerT() override = default;

        ExecutorsOwnerT(ExecutorsOwnerT&&) noexcept = default;
        ExecutorsOwnerT& operator=(ExecutorsOwnerT&&) noexcept = default;

        ExecutorsOwnerT(const ExecutorsOwnerT&) = delete;
        ExecutorsOwnerT& operator=(const ExecutorsOwnerT&) = delete;

        void Create() {
            resourceExec = std::make_unique<typename Backend::ResourceExecutor>();
            pipelineExec = std::make_unique<typename Backend::PipelineExecutor>();
            drawExec = std::make_unique<typename Backend::DrawExecutor>();
            swapChainExec = std::make_unique<typename Backend::SwapChainExecutor>();

            Backend::OnCreate(*this);

            raw.clear();
            if (resourceExec) raw.push_back(resourceExec.get());
            if (pipelineExec) raw.push_back(pipelineExec.get());
            if (drawExec) raw.push_back(drawExec.get());
            if (swapChainExec) raw.push_back(swapChainExec.get());
        }

        std::vector<IExecutorRHI*> GetIExecutors() const override { return raw; }

        void Clear() override { raw.clear(); }
        bool IsEmpty() const override { return raw.empty(); }

        Backend::ResourceExecutor* GetResourceExecutor() const { return resourceExec.get(); }
        Backend::PipelineExecutor* GetPipelineExecutor() const { return pipelineExec.get(); }
        Backend::DrawExecutor* GetDrawExecutor() const { return drawExec.get(); }
        Backend::SwapChainExecutor* GetSwapChainExecutor() const { return swapChainExec.get(); }
    };
}