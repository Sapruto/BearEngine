#pragma once

#include <memory>
#include <string>

#include "RHI/Base/Core/BaseContextInstanceRHI.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Fabric/IExecutorsOwner.h"

namespace RHI::FabricRHI {
    class ICreatorRHI {
    public:
        virtual ~ICreatorRHI() = default;

        virtual std::unique_ptr<Base::BaseContextInstanceRHI> CreateContext() const = 0;
        virtual std::unique_ptr<Base::BaseWindow> CreateWindow(const std::string&, uint32_t, uint32_t) const = 0;
        virtual std::unique_ptr<Base::BaseDevice> CreateDevice(std::shared_ptr<Base::BaseWindow>) const = 0;
        virtual std::unique_ptr<IExecutorsOwner> CreateExecutors() const = 0;
    };
}