#pragma once

#include <memory>
#include <string>

#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/Fabric/ICreatorRHI.h"

namespace RHI::FabricRHI {
    template<typename Backend>
    class CreatorRHI : public ICreatorRHI {
    public:
        std::unique_ptr<Base::BaseContextInstanceRHI> CreateContext() const override {
            return std::make_unique<typename Backend::ContextInstance>();
        }

        std::unique_ptr<Base::BaseWindow> CreateWindow(const std::string& title,
                                                        uint32_t w, uint32_t h) const override {
            auto window = std::make_unique<typename Backend::Window>();
            if (!window->Create(title, w, h)) return nullptr;
            return window;
        }

        std::unique_ptr<Base::BaseDevice> CreateDevice(std::shared_ptr<Base::BaseWindow> window) const override {
            auto native = std::dynamic_pointer_cast<typename Backend::Window>(window);
            if (!native) return nullptr;
            return std::make_unique<typename Backend::Device>(std::move(native));
        }

        std::unique_ptr<IExecutorsOwner> CreateExecutors() const override {
            auto owner = std::make_unique<ExecutorsOwnerT<Backend>>();
            owner->Create();
            return owner;
        }
    };
}