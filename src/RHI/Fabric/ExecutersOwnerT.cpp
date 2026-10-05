#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/Fabric/BackendTraits.h"

using namespace RHI;
using namespace FabricRHI;

template<>
void ExecutorsOwnerT<OpenGLBackend>::Create(BackendType type) {
    if (type == BackendType::USE_BASE) type = BackendType::OpenGL;
    impl->Clear();

    if (type == BackendType::OpenGL) {
        impl->bufferExec = std::make_unique<BufferExecutor>();
        impl->drawExec = std::make_unique<DrawExecutor>();
        impl->swapChainExec = std::make_unique<SwapChainExecutor>();
    }

    impl->RebuildRaw();
}

template<>
BackendType ExecutorsOwnerT<OpenGLBackend>::GetBackendType() const {
    return BackendType::OpenGL;
}