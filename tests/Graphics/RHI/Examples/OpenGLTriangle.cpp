#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include "RHI/RuntimeRHI.h"
#include "RHI/Fabric/BackendTraits.h"
#include "RHI/Fabric/ExecutorsOwnerT.h"

#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/PipelineExecutorParams.h"
#include "RHI/Base/Contracts/DrawExecutorParams.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"

#include "RHI/Base/Executors/ExecutorQueueRHI.h"
#include "RHI/MacrosRHI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace RHI;
using namespace RHI::Base;
using namespace RHI::Handles;
using namespace RHI::Executors;
using namespace RHI::Executors::ResourceExecutor;
using namespace RHI::Executors::PipelineExecutor;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::Executors::SwapChainExecutor;
using namespace RHI::FabricRHI;

constexpr uint32_t kWidth  = 800;
constexpr uint32_t kHeight = 600;

constexpr float kVertices[] = {
    -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
};

struct CameraUBO {
    float mvp[16];
};
static_assert(sizeof(CameraUBO) == 64, "CameraUBO must be 64 bytes");

constexpr const char* kVertexShader = R"(
#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 uMVP;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    vColor = aColor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

constexpr const char* kFragmentShader = R"(
#version 450 core

in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

static std::vector<std::byte> ToBytes(const char* str) {
    const auto len = std::strlen(str);
    std::vector<std::byte> data(len);
    std::memcpy(data.data(), str, len);
    return data;
}

template<typename T>
static std::vector<std::byte> ToBytes(const T& value) {
    std::vector<std::byte> data(sizeof(T));
    std::memcpy(data.data(), &value, sizeof(T));
    return data;
}

int main() {
    using Backend = OpenGLBackend;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(BackendType::OpenGL, "RHI Triangle", kWidth, kHeight)) {
        std::cerr << "[RHI] runtime init failed\n";
        return 1;
    }

    glfwSwapInterval(0);

    auto* device = rhi.Device();
    auto* window = rhi.Window();
    auto* queue  = rhi.Queue();

    auto* executorsBase = rhi.Executors();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(executorsBase);

    auto* resourceExec  = executors->GetResourceExecutor();
    auto* pipelineExec  = executors->GetPipelineExecutor();
    auto* drawExec      = executors->GetDrawExecutor();
    auto* swapChainExec = executors->GetSwapChainExecutor();

    for (auto* e : executors->GetIExecutors()) {
        queue->AddExecutor(e);
    }

    ShaderHandle vsHandle{};
    ShaderHandle fsHandle{};
    BufferHandle vboHandle{};
    BufferHandle uboHandle{};
    PipelineHandle pipelineHandle{};

    {
        ShaderDesc vsDesc{ ShaderStage::VERTEX, ToBytes(kVertexShader) };
        const uint64_t id = resourceExec->AddParamAndMark(
            ResourceParams{ CreateShaderParams{ vsDesc } }, 0, true);
        RHI_ONCE(resourceExec, id, CreateShaderResult, r, {
            vsHandle = r.handle;
        });
        queue->Execute();
    }

    {
        ShaderDesc fsDesc{ ShaderStage::FRAGMENT, ToBytes(kFragmentShader) };
        const uint64_t id = resourceExec->AddParamAndMark(
            ResourceParams{ CreateShaderParams{ fsDesc } }, 0, true);
        RHI_ONCE(resourceExec, id, CreateShaderResult, r, {
            fsHandle = r.handle;
        });
        queue->Execute();
    }

    {
        BufferDesc bd{ sizeof(kVertices), BufferUsage::VERTEX, MemoryType::DEVICE_LOCAL };
        const uint64_t id = resourceExec->AddParamAndMark(
            ResourceParams{ CreateBufferParams{ bd } }, 0, true);
        RHI_ONCE(resourceExec, id, CreateBufferResult, r, {
            vboHandle = r.handle;
        });
        queue->Execute();
    }

    {
        resourceExec->AddParamAndMark(
            ResourceParams{ UpdateBufferParams{
                vboHandle, 0, ToBytes(kVertices) } }, 0, true);
        queue->Execute();
    }

    {
        BufferDesc bd{ sizeof(CameraUBO), BufferUsage::UNIFORM, MemoryType::HOST_VISIBLE };
        const uint64_t id = resourceExec->AddParamAndMark(
            ResourceParams{ CreateBufferParams{ bd } }, 0, true);
        RHI_ONCE(resourceExec, id, CreateBufferResult, r, {
            uboHandle = r.handle;
        });
        queue->Execute();
    }

    CameraUBO camera{};
    camera.mvp[0]  = 1.0f;
    camera.mvp[5]  = 1.0f;
    camera.mvp[10] = 1.0f;
    camera.mvp[15] = 1.0f;

    {
        resourceExec->AddParamAndMark(
            ResourceParams{ UpdateBufferParams{
                uboHandle, 0, ToBytes(camera) } }, 0, true);
        queue->Execute();
    }

    {
        PipelineState state{};
        state.stages = {
            ShaderStageDesc{ ShaderStage::VERTEX,   vsHandle },
            ShaderStageDesc{ ShaderStage::FRAGMENT, fsHandle },
        };
        state.vertexAttributes = {
            { 0, 0, 0, Format::FLOAT3 },
            { 1, 0, sizeof(float) * 3, Format::FLOAT3 },
        };
        state.vertexBindings = {
            { 0, sizeof(float) * 6, false, 1 },
        };
        state.topology = PrimitiveTopology::TRIANGLE_LIST;
        state.cullMode = CullMode::NONE;
        state.depthTestEnable = false;
        state.depthWriteEnable = false;
        state.blendAttachments = { {} };
        state.colorFormats = { Format::UBYTE4N };
        state.depthStencilFormat = Format::FLOAT1;

        UniformBindingDesc ub{ 0, 0, "CameraUBO", sizeof(CameraUBO) };
        state.uniformBindings = { ub };

        const uint64_t id = pipelineExec->AddParamAndMark(
            PipelineParams{ CreatePipelineParams{ 0, state } }, 0, true);
        RHI_ONCE(pipelineExec, id, CreatePipelineResult, r, {
            pipelineHandle = r.handle;
        });
        queue->Execute();
    }

    {
        SwapChainCreateParams scp{ kWidth, kHeight, 3,
            PresentMode::IMMEDIATE, ColorSpace::SRGB_NONLINEAR };
        swapChainExec->AddParamAndMark(
            SwapChainParams{ scp }, 0, true);
        queue->Execute();
    }

    const VertexBufferBinding vbb{ vboHandle, 0 };

    {
        drawExec->AddParamAndMark(
            DrawParams{ BindUniformBufferParams{
                uboHandle, 0, 0, 0, sizeof(CameraUBO) } }, 0, true);
        queue->Execute();
    }

    double lastTime = glfwGetTime();
    int frameCount = 0;
    double fpsTimer = 0.0;
    double bestFps = 0.0;
    constexpr double kFpsUpdateInterval = 1.0;

    while (!window->ShouldClose()) {
        window->PollEvents();

        drawExec->AddParamAndMark(
            DrawParams{ BeginRenderPassParams{
                {},
                { 0, 0, static_cast<float>(kWidth), static_cast<float>(kHeight) },
                { true, true, true },
                0.05f, 0.05f, 0.1f, 1.0f,
                1.0f, 0 } }, 0, true);

        drawExec->AddParamAndMark(
            DrawParams{ DrawArraysParams{
                pipelineHandle,
                &vbb,
                1,
                DrawArraysParams::Plain{ 0, 3 } } }, 0, true);

        drawExec->AddParamAndMark(
            DrawParams{ EndRenderPassParams{} }, 0, true);

        swapChainExec->AddParamAndMark(
            SwapChainParams{ SwapChainPresentParams{ PresentMode::IMMEDIATE } }, 0, true);

        queue->Execute();

        ++frameCount;
        double now = glfwGetTime();
        fpsTimer += now - lastTime;
        lastTime = now;

        if (fpsTimer >= kFpsUpdateInterval) {
            double fps = frameCount / fpsTimer;
            if (fps > bestFps) bestFps = fps;

            std::cout << "FPS: " << static_cast<int>(fps)
                        << " | best: " << static_cast<int>(bestFps)
                        << " | frame: " << (fpsTimer / frameCount * 1000.0) << " ms\n";

            frameCount = 0;
            fpsTimer = 0.0;
        }
    }

    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyBufferParams{ vboHandle } }, 0, true);
    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyBufferParams{ uboHandle } }, 0, true);
    pipelineExec->AddParamAndMark(
        PipelineParams{ DestroyPipelineParams{ 0, pipelineHandle } }, 0, true);
    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyShaderParams{ vsHandle } }, 0, true);
    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyShaderParams{ fsHandle } }, 0, true);

    swapChainExec->AddParamAndMark(
        SwapChainParams{ SwapChainDestroyParams{} }, 0, true);

    queue->Execute();

    for (auto* e : executors->GetIExecutors()) queue->RemoveExecutor(e);
    queue->Clear();

    rhi.Shutdown();
    return 0;
}