#include "RHI/Fabric/ExecutersOwnerT.h"
#include "RHI/Fabric/BackendTraits.h"

namespace FabricRHI {
    template<>
    void ExecutersOwnerT<OpenGLBackend>::Create(BackendType type) {
        if (type == BackendType::USE_BASE) type = BackendType::OpenGL;
        impl->Clear();

        if (type == BackendType::OpenGL) {
            impl->bufferExec    = std::make_unique<BufferExecuter>();
            impl->drawExec      = std::make_unique<DrawExecuter>();
            impl->swapChainExec = std::make_unique<SwapChainExecuter>();
        }

        impl->RebuildRaw();
    }

    template<>
    BackendType ExecutersOwnerT<OpenGLBackend>::GetBackendType() const {
        return BackendType::OpenGL;
    }
}