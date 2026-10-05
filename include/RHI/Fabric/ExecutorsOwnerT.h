#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <utility>

#include "RHI/Fabric/IExecutorsOwner.h"
#include "RHI/Fabric/ExecutorVariants.h"

namespace RHI::FabricRHI {
    using RHI::Executors::IExecutorRHI;
    using RHI::Executors::ExecutorTypeRHI;

    template<typename Backend>
    class ExecutorsOwnerT : public IExecutorsOwner {
    public:
        using BufferExecutor = typename Backend::BufferExecutor;
        using DrawExecutor = typename Backend::DrawExecutor;
        using SwapChainExecutor = typename Backend::SwapChainExecutor;

        using BufferVariant = BufferExecutorVariant<Backend>;
        using DrawVariant = DrawExecutorVariant<Backend>;
        using SwapChainVariant = SwapChainExecutorVariant<Backend>;
        using AnyVariant = AnyExecutorVariant<Backend>;

    private:
        struct Impl {
            std::unique_ptr<BufferExecutor> bufferExec;
            std::unique_ptr<DrawExecutor> drawExec;
            std::unique_ptr<SwapChainExecutor> swapChainExec;

            std::vector<IExecutorRHI*> raw;

            void RebuildRaw() {
                raw.clear();
                if (bufferExec) raw.push_back(bufferExec.get());
                if (drawExec) raw.push_back(drawExec.get());
                if (swapChainExec) raw.push_back(swapChainExec.get());
            }

            void Clear() {
                raw.clear();
                bufferExec.reset();
                drawExec.reset();
                swapChainExec.reset();
            }
        };

        std::unique_ptr<Impl> impl;

    public:
        ExecutorsOwnerT() : impl(std::make_unique<Impl>()) {}
        ~ExecutorsOwnerT() override = default;

        ExecutorsOwnerT(ExecutorsOwnerT&&) noexcept = default;
        ExecutorsOwnerT& operator=(ExecutorsOwnerT&&) noexcept = default;

        ExecutorsOwnerT(const ExecutorsOwnerT&) = delete;
        ExecutorsOwnerT& operator=(const ExecutorsOwnerT&) = delete;

        void Create(BackendType type = BackendType::USE_BASE);

        std::vector<IExecutorRHI*> GetIExecutors() const override { return impl->raw; }

        IExecutorRHI* GetIExecutor(ExecutorTypeRHI type) const override {
            for (auto* e : impl->raw)
                if (e && e->GetExecutorType() == type) return e;
            return nullptr;
        }

        IExecutorRHI* GetExecutorRaw(ExecutorTypeRHI type) const override {
            return GetIExecutor(type);
        }

        void Clear() override { impl->Clear(); }
        bool IsEmpty() const override { return impl->raw.empty(); }
        BackendType GetBackendType() const override;

        BufferExecutor* GetBufferExecutor() const { return impl->bufferExec.get(); }
        DrawExecutor* GetDrawExecutor() const { return impl->drawExec.get(); }
        SwapChainExecutor* GetSwapChainExecutor() const { return impl->swapChainExec.get(); }

        template<typename T>
        T* GetExecutorAs(ExecutorTypeRHI type) const {
            AnyVariant v = GetExecutor(type);
            if (auto* pp = std::get_if<T*>(&v)) {
                return *pp;
            }
            return nullptr;
        }

        AnyVariant GetExecutor(ExecutorTypeRHI type) const {
            switch (type) {
                case ExecutorTypeRHI::BufferExecutor:
                    if (impl->bufferExec) return impl->bufferExec.get();
                    return std::monostate{};
                case ExecutorTypeRHI::DrawExecutor:
                    if (impl->drawExec) return impl->drawExec.get();
                    return std::monostate{};
                case ExecutorTypeRHI::SwapChainExecutor:
                    if (impl->swapChainExec) return impl->swapChainExec.get();
                    return std::monostate{};
                default:
                    return std::monostate{};
            }
        }
    };
}