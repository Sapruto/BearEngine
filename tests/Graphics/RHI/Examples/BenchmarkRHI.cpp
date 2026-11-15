#include <chrono>
#include <cstring>
#include <iostream>
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

using namespace RHI;
using namespace RHI::Base;
using namespace RHI::Handles;
using namespace RHI::Executors;
using namespace RHI::Executors::ResourceExecutor;
using namespace RHI::Executors::PipelineExecutor;
using namespace RHI::Executors::DrawExecutor;
using namespace RHI::Executors::SwapChainExecutor;
using namespace RHI::FabricRHI;

constexpr uint32_t kWidth  = 1920;
constexpr uint32_t kHeight = 1080;

constexpr const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

constexpr const char* kFragmentShader = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

static std::vector<std::byte> ToBytes(const char* s) {
    const auto n = std::strlen(s);
    std::vector<std::byte> b(n);
    std::memcpy(b.data(), s, n);
    return b;
}

using Clock = std::chrono::high_resolution_clock;
using ms = std::chrono::duration<double, std::milli>;

struct ScopedTimer {
    const char* name;
    Clock::time_point start;
    ScopedTimer(const char* n) : name(n), start(Clock::now()) {}
    ~ScopedTimer() {
        const auto end = Clock::now();
        std::cout << "[BENCH] " << name << ": "
                << ms(end - start).count() << " ms\n";
    }
};

int main() {
    using Backend = OpenGLBackend;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(BackendType::OpenGL, "RHI Benchmark", kWidth, kHeight)) {
        std::cerr << "[RHI] init failed\n";
        return 1;
    }

    auto* device = rhi.Device();
    auto* window = rhi.Window();
    auto* queue  = rhi.Queue();

    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());
    auto* resourceExec  = executors->GetResourceExecutor();
    auto* pipelineExec  = executors->GetPipelineExecutor();
    auto* drawExec      = executors->GetDrawExecutor();
    auto* swapChainExec = executors->GetSwapChainExecutor();

    ShaderHandle vs{}, fs{};
    {
        ShaderDesc d; d.stage = ShaderStage::VERTEX; d.bytecode = ToBytes(kVertexShader);
        const auto id = resourceExec->AddParamAndMark(ResourceParams{ CreateShaderParams{ d } }, 0, true);
        resourceExec->SubscribeOnce(id, [&](const auto& r) {
            if (!r.empty()) if (auto* x = std::get_if<CreateShaderResult>(&r[0].data)) vs = x->handle;
        });
        queue->Execute();
    }
    {
        ShaderDesc d; d.stage = ShaderStage::FRAGMENT; d.bytecode = ToBytes(kFragmentShader);
        const auto id = resourceExec->AddParamAndMark(ResourceParams{ CreateShaderParams{ d } }, 0, true);
        resourceExec->SubscribeOnce(id, [&](const auto& r) {
            if (!r.empty()) if (auto* x = std::get_if<CreateShaderResult>(&r[0].data)) fs = x->handle;
        });
        queue->Execute();
    }

    const float kTri[] = {
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f,
        0.0f,  0.5f, 0.f,
    };

    BufferHandle vbo{};
    {
        BufferDesc bd; bd.size = sizeof(kTri); bd.usage = BufferUsage::VERTEX;
        const auto id = resourceExec->AddParamAndMark(ResourceParams{ CreateBufferParams{ bd } }, 0, true);
        resourceExec->SubscribeOnce(id, [&](const auto& r) {
            if (!r.empty()) if (auto* x = std::get_if<CreateBufferResult>(&r[0].data)) vbo = x->handle;
        });
        queue->Execute();
    }
    {
        UpdateBufferParams up; up.handle = vbo;
        up.data.resize(sizeof(kTri));
        std::memcpy(up.data.data(), kTri, sizeof(kTri));
        resourceExec->AddParamAndMark(ResourceParams{ up }, 0, true);
        queue->Execute();
    }

    PipelineHandle pipe{};
    {
        PipelineState st;
        st.stages = { { ShaderStage::VERTEX, vs, "main" },
                    { ShaderStage::FRAGMENT, fs, "main" } };
        st.vertexAttributes = { { 0, 0, 0, Format::FLOAT3 } };
        st.vertexBindings = { { 0, sizeof(float) * 3, false, 1 } };
        st.cullMode = CullMode::NONE;
        st.depthTestEnable = false;
        st.depthWriteEnable = false;
        st.blendAttachments = { {} };

        const auto id = pipelineExec->AddParamAndMark(
            PipelineParams{ CreatePipelineParams{ 0, st } }, 0, true);
        pipelineExec->SubscribeOnce(id, [&](const auto& r) {
            if (!r.empty()) if (auto* x = std::get_if<CreatePipelineResult>(&r[0].data)) pipe = x->handle;
        });
        queue->Execute();
    }

    {
        SwapChainCreateParams scp;
        scp.width = kWidth; scp.height = kHeight;
        scp.bufferCount = 2; scp.presentMode = PresentMode::FIFO;
        swapChainExec->AddParamAndMark(SwapChainParams{ scp }, 0, true);
        queue->Execute();
    }

    constexpr int kDrawCalls = 100000;
    constexpr int kFrames    = 60;

    std::cout << "\n=== BENCH 1: " << kDrawCalls << " draw calls x "
            << kFrames << " frames ===\n";

    const VertexBufferBinding vbb{ vbo, 0 };

    {
        drawExec->ReserveParams((kDrawCalls + 8) * kFrames);

        ScopedTimer t("Draw stress total");
        for (int frame = 0; frame < kFrames; ++frame) {
            window->PollEvents();

            BeginRenderPassParams rp;
            rp.renderTarget = {};
            rp.renderArea = { 0, 0, static_cast<float>(kWidth), static_cast<float>(kHeight) };
            rp.clearFlags = BeginRenderPassParams::ClearFlags{ true, true, true };
            rp.clearColorR = 0.05f;
            rp.clearColorG = 0.05f;
            rp.clearColorB = 0.1f;
            rp.clearColorA = 1.0f;
            rp.clearDepth = 1.0f;
            rp.clearStencil = 0;
            drawExec->AddParam(DrawParams{ rp }, 0, true);

            for (int i = 0; i < kDrawCalls; ++i) {
                DrawArraysParams dp;
                dp.pipeline = pipe;
                dp.vertexBuffers = &vbb;
                dp.vertexBufferCount = 1;
                dp.data = DrawArraysParams::Plain{ 0, 3 };
                drawExec->AddParam(DrawParams{ dp }, 0, true);
            }

            drawExec->AddParam(DrawParams{ EndRenderPassParams{} }, 0, true);

            swapChainExec->AddParam(SwapChainParams{
                SwapChainPresentParams{ PresentMode::FIFO } }, 0, true);

            queue->Execute();
        }
    }

    constexpr int kBufferIterations = 10000;
    std::cout << "\n=== BENCH 2: " << kBufferIterations
            << " buffer create/update/destroy ===\n";

    {
        ScopedTimer t("Buffer churn");

        std::vector<std::byte> payload(1024);
        std::memset(payload.data(), 0xAB, payload.size());

        for (int i = 0; i < kBufferIterations; ++i) {
            BufferDesc bd;
            bd.size = payload.size();
            bd.usage = BufferUsage::VERTEX;

            BufferHandle h{};
            const auto createId = resourceExec->AddParam(
                ResourceParams{ CreateBufferParams{ bd } }, 0, true);
            resourceExec->SubscribeOnce(createId, [&](const auto& r) {
                if (!r.empty()) if (auto* x = std::get_if<CreateBufferResult>(&r[0].data)) h = x->handle;
            });

            UpdateBufferParams up; up.handle = h; up.data = payload;
            resourceExec->AddParam(ResourceParams{ up }, 0, true);

            resourceExec->AddParam(
                ResourceParams{ DestroyBufferParams{ h } }, 0, true);

            resourceExec->MarkProcess(true);
            queue->Execute();
        }
    }

    constexpr int kPipelineIterations = 1000;
    std::cout << "\n=== BENCH 3: " << kPipelineIterations
            << " pipeline create/destroy ===\n";

    {
        ScopedTimer t("Pipeline churn");

        for (int i = 0; i < kPipelineIterations; ++i) {
            PipelineState st;
            st.stages = { { ShaderStage::VERTEX, vs, "main" },
                        { ShaderStage::FRAGMENT, fs, "main" } };
            st.vertexAttributes = { { 0, 0, 0, Format::FLOAT3 } };
            st.vertexBindings = { { 0, sizeof(float) * 3, false, 1 } };
            st.cullMode = (i % 2 == 0) ? CullMode::NONE : CullMode::BACK;
            st.depthTestEnable = false;
            st.depthWriteEnable = false;
            st.blendAttachments = { {} };

            PipelineHandle ph{};
            const auto id = pipelineExec->AddParamAndMark(
                PipelineParams{ CreatePipelineParams{ 0, st } }, 0, true);
            pipelineExec->SubscribeOnce(id, [&](const auto& r) {
                if (!r.empty()) if (auto* x = std::get_if<CreatePipelineResult>(&r[0].data)) ph = x->handle;
            });
            queue->Execute();

            pipelineExec->AddParamAndMark(
                PipelineParams{ DestroyPipelineParams{ 0, ph } }, 0, true);
            queue->Execute();
        }
    }

    constexpr int kQueueOps = 1000000;
    std::cout << "\n=== BENCH 4: " << kQueueOps
            << " queue AddParam + Execute ===\n";

    {
        drawExec->ReserveParams(kQueueOps + 8);

        ScopedTimer t("Queue throughput");

        for (int i = 0; i < kQueueOps; ++i) {
            DrawArraysParams dp;
            dp.pipeline = pipe;
            dp.vertexBuffers = &vbb;
            dp.vertexBufferCount = 1;
            dp.data = DrawArraysParams::Plain{ 0, 3 };

            drawExec->AddParamDirect<DrawParams>(DrawParams{ dp }, 0, true);
        }
    }

    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyBufferParams{ vbo } }, 0, true);
    pipelineExec->AddParamAndMark(
        PipelineParams{ DestroyPipelineParams{ 0, pipe } }, 0, true);
    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyShaderParams{ vs } }, 0, true);
    resourceExec->AddParamAndMark(
        ResourceParams{ DestroyShaderParams{ fs } }, 0, true);
    swapChainExec->AddParamAndMark(
        SwapChainParams{ SwapChainDestroyParams{} }, 0, true);
    queue->Execute();

    rhi.Shutdown();
    return 0;
}