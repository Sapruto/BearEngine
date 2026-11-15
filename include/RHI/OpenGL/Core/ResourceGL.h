#pragma once

#include "RHI/Base/Core/BaseResourceStore.h"
#include "RHI/OpenGL/HandlesGL.h"

namespace RHI::OpenGL {
    class ResourceGL final : public Resource::BaseResourceStore<NativeHandleGL> {

    };
}