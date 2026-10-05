#pragma once

#include <any>
#include <memory>
#include <string>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/IExecutorsOwner.h"
#include "RHI/Fabric/ExecutorsOwnerT.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Base/Core/BaseShader.h"
#include "RHI/Base/Core/BaseContextInstanceRHI.h"

namespace RHI::FabricRHI {
    namespace B = RHI::Base;

    class CreatorRHI {
    private:
        BackendType backend;

    public:
        explicit CreatorRHI(BackendType type = BackendType::USE_BASE);
        ~CreatorRHI();

        CreatorRHI(CreatorRHI&&) noexcept;
        CreatorRHI& operator=(CreatorRHI&&) noexcept;
        CreatorRHI(const CreatorRHI&) = delete;
        CreatorRHI& operator=(const CreatorRHI&) = delete;

        void SetBackend(BackendType type);
        BackendType GetBackend() const;

        std::unique_ptr<B::BaseContextInstanceRHI> CreateContext() const;

        std::unique_ptr<B::BaseWindow> CreateWindow(const std::string& title,
                                                unsigned int width,
                                                unsigned int height) const;

        std::unique_ptr<B::BaseDevice> CreateDevice(std::shared_ptr<B::BaseWindow> window) const;

        std::unique_ptr<B::BaseShader> CreateShader(const std::string& vertexPath,
                                                const std::string& fragmentPath) const;

        std::unique_ptr<IExecutorsOwner> CreateExecutors() const;
        template<typename Backend>
        std::unique_ptr<ExecutorsOwnerT<Backend>> CreateExecutorsT() const {
            auto owner = std::make_unique<ExecutorsOwnerT<Backend>>();
            owner->Create(Backend::kType);
            return owner;
        }

        void* WindowNativeHandleToVoid(const std::any& native) const;
    };
}