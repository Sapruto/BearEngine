#pragma once

#include <any>
#include <memory>
#include <string>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/IExecutersOwner.h"

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Base/Core/BaseShader.h"
#include "RHI/Base/Core/BaseContextInstanceRHI.h"

namespace FabricRHI {
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

        std::unique_ptr<BaseContextInstanceRHI> CreateContext() const;

        std::unique_ptr<BaseWindow> CreateWindow(const std::string& title,
                                                unsigned int width,
                                                unsigned int height) const;

        std::unique_ptr<BaseDevice> CreateDevice(std::shared_ptr<BaseWindow> window) const;

        std::unique_ptr<BaseShader> CreateShader(const std::string& vertexPath,
                                                const std::string& fragmentPath) const;

        std::unique_ptr<IExecutersOwner> CreateExecuters() const;

        void* WindowNativeHandleToVoid(const std::any& native) const;
    };
}