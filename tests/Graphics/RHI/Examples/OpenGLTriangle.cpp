#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "RHI/RuntimeRHI.h"

#include "RHI/Fabric/CreatorRHI.h"
#include "RHI/Fabric/BackendTraits.h"

#include "RHI/Base/Models/BufferExecutorParams.h"
#include "RHI/Base/Models/BufferExecutorResult.h"
#include "RHI/Base/Models/DrawExecuteParams.h"
#include "RHI/Base/Models/DrawExecuteResult.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

#include "RHI/Base/Executors/ExecutorQueueRHI.h"

using namespace RHI;
using namespace Base;
using namespace Executors;
using namespace FabricRHI;
using namespace BufferExecute;
using namespace DrawExecute;
using namespace SwapChainExecute;

#ifndef RHI_SHADER_DIR
#define RHI_SHADER_DIR "shaders"
#endif

namespace {
    constexpr float kVertices[] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,

        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    };

    constexpr uint32_t kWidth  = 800;
    constexpr uint32_t kHeight = 600;
    constexpr const char* kWindowTitle = "RHI Triangle";
}

int main() {
    using Backend = OpenGLBackend;

    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(BackendType::OpenGL, kWindowTitle, kWidth, kHeight)) {
        std::cerr << "[RHI] runtime init failed\n";
        return 1;
    }

    BaseWindow* window = rhi.Window();
    BaseDevice* device = rhi.Device();

    const std::string vsPath = std::string(RHI_SHADER_DIR) + "/tri.vert";
    const std::string fsPath = std::string(RHI_SHADER_DIR) + "/tri.frag";

    auto shader = rhi.CreateShader(vsPath, fsPath);
    if (!shader || !shader->IsReady()) {
        std::cerr << "[RHI] shader compile failed\n";
        rhi.Shutdown();
        return 1;
    }
    const uint64_t programID = shader->GetNativeHandle();

    std::shared_ptr<BaseDevice> deviceView(device, [](BaseDevice*) {});
    ExecutorQueueRHI queue(deviceView);

    auto* executers = rhi.ExecutorsT<Backend>();
    if (!executers || executers->IsEmpty()) {
        std::cerr << "[RHI] failed to create executers\n";
        rhi.Shutdown();
        return 1;
    }

    auto* bufferExec = executers->GetExecutorAs<typename Backend::BufferExecutor>(
                                ExecutorTypeRHI::BufferExecutor);
    auto* drawExec = executers->GetExecutorAs<typename Backend::DrawExecutor>(
                                ExecutorTypeRHI::DrawExecutor);
    auto* swapChainExec = executers->GetExecutorAs<typename Backend::SwapChainExecutor>(
                                ExecutorTypeRHI::SwapChainExecutor);

    if (!bufferExec || !drawExec || !swapChainExec) {
        std::cerr << "[RHI] failed to obtain executers\n";
        rhi.Shutdown();
        return 1;
    }

    for (auto* e : executers->GetIExecutors()) {
        queue.AddExecutor(e);
    }

    auto submit = [](auto* exec, auto param, int layer = 0) {
        return exec->AddParamAndMark(param, layer, true);
    };

    unsigned int vbo = 0;
    unsigned int vao = 0;

    {
        VBOCreateParams p(kVertices, sizeof(kVertices), BufferUsage::Static, true, true);
        const uint64_t id = submit(bufferExec, BufferParams{p});

        bufferExec->SubscribeOnce(id,
            [&vbo](const std::vector<BufferExecuteResult>& results) {
                if (results.empty()) return;
                if (auto* res = std::get_if<VBOCreateResult>(&results[0].data)) {
                    vbo = res->vboID;
                }
            });

        queue.Execute();
    }

    {
        VAOCreateParams p(vbo, 0, true, true);
        const uint64_t id = submit(bufferExec, BufferParams{p});

        bufferExec->SubscribeOnce(id,
            [&vao](const std::vector<BufferExecuteResult>& results) {
                if (results.empty()) return;
                if (auto* res = std::get_if<VAOCreateResult>(&results[0].data)) {
                    vao = res->vaoID;
                }
            });

        queue.Execute();
    }

    {
        VAOSetAttributeParams p(vao, vbo, 0, AttributeType::Float3,
                                0, sizeof(float) * 6, false, false, true, 0);
        submit(bufferExec, BufferParams{p});
    }
    {
        VAOSetAttributeParams p(vao, vbo, 1, AttributeType::Float3,
                                sizeof(float) * 3, sizeof(float) * 6, false, false, true, 0);
        submit(bufferExec, BufferParams{p});
    }
    queue.Execute();

    {
        SwapChainCreateParams p(nullptr, kWidth, kHeight, 2, IntervalType::FIFO, 0.0f);
        submit(swapChainExec, SwapChainParams{p});
        queue.Execute();
    }

    device->SetClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    while (!window->ShouldClose()) {
        window->PollEvents();
        device->Clear(ClearFlagsRHI::All);

        {
            DrawArrays dp(DrawMode::TRIANGLES, 0, 6, vao, programID);
            submit(drawExec, DrawParams{dp});
        }
        {
            SwapChainPresentParams p(1, IntervalType::FIFO);
            submit(swapChainExec, SwapChainParams{p});
        }

        queue.Execute();
    }

    {
        BufferDestroyParams p(vbo, BufferType::VBO);
        submit(bufferExec, BufferParams{p});

        SwapChainDestroyParams p2(false);
        submit(swapChainExec, SwapChainParams{p2});

        queue.Execute();
    }

    for (auto* e : executers->GetIExecutors()) {
        queue.RemoveExecutor(e);
    }
    queue.Clear();

    shader.reset();

    rhi.Shutdown();
    return 0;
}