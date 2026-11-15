#pragma once

#include <memory>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/ICreatorRHI.h"
#include "RHI/Fabric/BackendTraits.h"
#include "RHI/Fabric/CreatorRHI.h"

namespace RHI::FabricRHI {
    inline std::unique_ptr<ICreatorRHI> MakeRHI(BackendType type) {
        switch (type) {
            case BackendType::OpenGL: return std::make_unique<CreatorRHI<OpenGLBackend>>();
            //TODO: case BackendType::Vulkan: return std::make_unique<CreatorRHI<VulkanBackend>>();
            default: return std::make_unique<CreatorRHI<OpenGLBackend>>(); //? DEFAULT OpenGL why not?
        }
    }
}