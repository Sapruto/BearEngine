#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <utility>

#include "RHI/Fabric/IExecutersOwner.h"
#include "RHI/Fabric/ExecuterVariants.h"

namespace FabricRHI {
    template<typename Backend>
    class ExecutersOwnerT : public IExecutersOwner {
    public:
        using BufferExecuter = typename Backend::BufferExecuter;
        using DrawExecuter = typename Backend::DrawExecuter;
        using SwapChainExecuter = typename Backend::SwapChainExecuter;

        using BufferVariant = BufferExecuterVariant<Backend>;
        using DrawVariant = DrawExecuterVariant<Backend>;
        using SwapChainVariant = SwapChainExecuterVariant<Backend>;
        using AnyVariant = AnyExecuterVariant<Backend>;

    private:
        struct Impl {
            std::unique_ptr<BufferExecuter> bufferExec;
            std::unique_ptr<DrawExecuter> drawExec;
            std::unique_ptr<SwapChainExecuter> swapChainExec;

            std::vector<IExecuterRHI*> raw;

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
        ExecutersOwnerT() : impl(std::make_unique<Impl>()) {}
        ~ExecutersOwnerT() override = default;

        ExecutersOwnerT(ExecutersOwnerT&&) noexcept = default;
        ExecutersOwnerT& operator=(ExecutersOwnerT&&) noexcept = default;

        ExecutersOwnerT(const ExecutersOwnerT&) = delete;
        ExecutersOwnerT& operator=(const ExecutersOwnerT&) = delete;

        void Create(BackendType type = BackendType::USE_BASE);

        std::vector<IExecuterRHI*> GetIExecuters() const override { return impl->raw; }

        IExecuterRHI* GetIExecuter(ExecuterTypeRHI type) const override {
            for (auto* e : impl->raw)
                if (e && e->GetExecuterType() == type) return e;
            return nullptr;
        }

        IExecuterRHI* GetExecuterRaw(ExecuterTypeRHI type) const override {
            return GetIExecuter(type);
        }

        void Clear() override { impl->Clear(); }
        bool IsEmpty() const override { return impl->raw.empty(); }
        BackendType GetBackendType() const override;

        BufferExecuter* GetBufferExecuter() const { return impl->bufferExec.get(); }
        DrawExecuter* GetDrawExecuter() const { return impl->drawExec.get(); }
        SwapChainExecuter* GetSwapChainExecuter() const { return impl->swapChainExec.get(); }

        template<typename T>
        T* GetExecuterAs(ExecuterTypeRHI type) const {
            AnyVariant v = GetExecuter(type);
            if (auto* pp = std::get_if<T*>(&v)) {
                return *pp;
            }
            return nullptr;
        }

        AnyVariant GetExecuter(ExecuterTypeRHI type) const {
            switch (type) {
                case ExecuterTypeRHI::BufferExecuter:
                    if (impl->bufferExec) return impl->bufferExec.get();
                    return std::monostate{};
                case ExecuterTypeRHI::DrawExecuter:
                    if (impl->drawExec) return impl->drawExec.get();
                    return std::monostate{};
                case ExecuterTypeRHI::SwapChainExecuter:
                    if (impl->swapChainExec) return impl->swapChainExec.get();
                    return std::monostate{};
                default:
                    return std::monostate{};
            }
        }
    };
}