#pragma once

#include "RHI/Fabric/FabricTypes.h"

#include "RHI/OpenGL/Core/ContextInstanceGL.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ShaderGL.h"

#include "RHI/OpenGL/Executors/BufferExecutorGL.h"
#include "RHI/OpenGL/Executors/DrawExecutorGL.h"
#include "RHI/OpenGL/Executors/SwapChainExecutorGL.h"

namespace RHI::FabricRHI {
    struct OpenGLBackend {
        using ContextInstance = RHI::OpenGL::ContextInstanceGL;
        using Window = RHI::OpenGL::WindowGL;
        using Device = RHI::OpenGL::DeviceGL;
        using Shader = RHI::OpenGL::ShaderGL;

        using BufferExecutor = RHI::Executors::BufferExecute::BufferExecutorGL;
        using DrawExecutor = RHI::Executors::DrawExecute::DrawExecutorGL;
        using SwapChainExecutor = RHI::Executors::SwapChainExecute::SwapChainExecutorGL;

        static constexpr BackendType kType = BackendType::OpenGL;
    };

    using CurrentBackend = OpenGLBackend;
}