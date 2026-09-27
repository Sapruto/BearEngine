#pragma once

#include "RHI/Fabric/FabricTypes.h"

#include "RHI/OpenGL/Core/ContextInstanceGL.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ShaderGL.h"

#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "RHI/OpenGL/Executers/DrawExecuterGL.h"
#include "RHI/OpenGL/Executers/SwapChainExecuterGL.h"

namespace FabricRHI {
    struct OpenGLBackend {
        using ContextInstance = ContextInstanceGL;
        using Window = WindowGL;
        using Device = DeviceGL;
        using Shader = ShaderGL;

        using BufferExecuter = BufferExecute::BufferExecuterGL;
        using DrawExecuter = DrawExecute::DrawExecuterGL;
        using SwapChainExecuter = SwapChainExecute::SwapChainExecuterGL;

        static constexpr BackendType kType = BackendType::OpenGL;
    };
}