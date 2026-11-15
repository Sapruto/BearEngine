#pragma once

#include "RHI/OpenGL/Core/ContextInstanceGL.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"

#include "RHI/OpenGL/Executors/DrawExecutorGL.h"
#include "RHI/OpenGL/Executors/PipelineExecutorGL.h"
#include "RHI/OpenGL/Executors/ResourceExecutorGL.h"
#include "RHI/OpenGL/Executors/SwapChainExecutorGL.h"

#include "RHI/Fabric/ExecutorsOwnerT.h"

namespace RHI::FabricRHI {
    struct OpenGLBackend {
        using ContextInstance = RHI::OpenGL::ContextInstanceGL;
        using Window = RHI::OpenGL::WindowGL;
        using Device = RHI::OpenGL::DeviceGL;
        
        using ResourceExecutor = RHI::Executors::ResourceExecutor::ResourceExecutorGL;
        using PipelineExecutor = RHI::Executors::PipelineExecutor::PipelineExecutorGL;
        using DrawExecutor = RHI::Executors::DrawExecutor::DrawExecutorGL;
        using SwapChainExecutor = RHI::Executors::SwapChainExecutor::SwapChainExecutorGL;
        
        template<typename Backend>
        static void OnCreate(ExecutorsOwnerT<Backend>& owner) {
            owner.GetDrawExecutor()->SetPipelineExecutor(owner.GetPipelineExecutor());
        }
    };
}