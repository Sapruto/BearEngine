#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "RHI/Fabric/CreatorRHI.h"
#include "RHI/Fabric/IExecutersOwner.h"

#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "RHI/OpenGL/Executers/DrawExecuterGL.h"
#include "RHI/OpenGL/Executers/SwapChainExecuterGL.h"

#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"
#include "RHI/Base/Models/DrawExecuteParams.h"
#include "RHI/Base/Models/DrawExecuteResult.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

#include "RHI/ExecuterQueueRHI.h"

using namespace FabricRHI;
using namespace BufferExecute;
using namespace DrawExecute;
using namespace SwapChainExecute;

#ifndef RHI_SHADER_DIR
#define RHI_SHADER_DIR "shaders"
#endif

static const float kVertices[] = {
    -1.0f, -1.0f, 0.0f,  0,0,0,
     1.0f, -1.0f, 0.0f,  0,0,0,
     1.0f,  1.0f, 0.0f,  0,0,0,

    -1.0f, -1.0f, 0.0f,  0,0,0,
     1.0f,  1.0f, 0.0f,  0,0,0,
    -1.0f,  1.0f, 0.0f,  0,0,0,
};

int main() {
    CreatorRHI rhi(BackendType::OpenGL);

    auto context = rhi.CreateContext();
    if (!context || !context->Initialize()) {
        std::cerr << "[RHI] context init failed\n";
        return 1;
    }

    auto windowUnique = rhi.CreateWindow("RHI Triangle", 800, 600);
    if (!windowUnique) {
        std::cerr << "[RHI] window create failed\n";
        return 1;
    }
    std::shared_ptr<BaseWindow> window = std::move(windowUnique);

    auto deviceUnique = rhi.CreateDevice(window);
    if (!deviceUnique || !deviceUnique->Initialize()) {
        std::cerr << "[RHI] device init failed\n";
        return 1;
    }
    std::shared_ptr<BaseDevice> device = std::move(deviceUnique);

    const std::string vsPath = std::string(RHI_SHADER_DIR) + "/tri.vert";
    const std::string fsPath = std::string(RHI_SHADER_DIR) + "/tri.frag";

    auto shader = rhi.CreateShader(vsPath, fsPath);
    if (!shader || !shader->IsReady()) {
        std::cerr << "[RHI] shader compile failed\n";
        return 1;
    }
    const uint64_t programID = shader->GetNativeHandle();

    ExecuterQueueRHI queue(device);

    auto executers = rhi.CreateExecuters();
    if (!executers || executers->IsEmpty()) {
        std::cerr << "[RHI] failed to create executers\n";
        return 1;
    }

    auto* bufferExec = dynamic_cast<BufferExecuterGL*>(
        executers->GetExecuterRaw(ExecuterTypeRHI::BufferExecuter));
    auto* drawExec = dynamic_cast<DrawExecuterGL*>(
        executers->GetExecuterRaw(ExecuterTypeRHI::DrawExecuter));
    auto* swapChainExec = dynamic_cast<SwapChainExecuterGL*>(
        executers->GetExecuterRaw(ExecuterTypeRHI::SwapChainExecuter));

    if (!bufferExec || !drawExec || !swapChainExec) {
        std::cerr << "[RHI] failed to obtain executers\n";
        return 1;
    }

    bufferExec->SetLayer(0);
    drawExec->SetLayer(1);
    swapChainExec->SetLayer(2);

    for (auto* e : executers->GetIExecuters()) {
        queue.AddExecuter(e);
    }

    unsigned int vbo = 0;
    unsigned int vao = 0;

    {
        VBOCreateParams p(kVertices, sizeof(kVertices), BufferUsage::Static, true, true);
        int id = bufferExec->AddParamAndMark(BufferParams{p});
        bufferExec->SubscribeOnce(id, [&](const std::vector<BufferExecuteResult>& r){
            if (r.empty()) return;
            if (auto* res = std::get_if<VBOCreateResult>(&r[0].data)) {
                vbo = res->vboID;
            }
        });
        queue.Execute();
    }

    {
        VAOCreateParams p(vbo, 0, true, true);
        int id = bufferExec->AddParamAndMark(BufferParams{p});
        bufferExec->SubscribeOnce(id, [&](const std::vector<BufferExecuteResult>& r){
            if (r.empty()) return;
            if (auto* res = std::get_if<VAOCreateResult>(&r[0].data)) {
                vao = res->vaoID;
            }
        });
        queue.Execute();
    }

    {
        VAOSetAttributeParams p(vao, vbo, 0, AttributeType::Float3, 0,
                                sizeof(float) * 6, false, false, true, 0);
        bufferExec->AddParamAndMark(BufferParams{p});
        queue.Execute();
    }

    {
        VAOSetAttributeParams p(vao, vbo, 1, AttributeType::Float3,
                                sizeof(float) * 3, sizeof(float) * 6, false, false, true, 0);
        bufferExec->AddParamAndMark(BufferParams{p});
        queue.Execute();
    }

    {
        SwapChainCreateParams p(nullptr, 800, 600, 2, IntervalType::FIFO, 0.0f);
        swapChainExec->AddParamAndMark(SwapChainParams{p});
        queue.Execute();
    }

    device->SetClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    while (!window->ShouldClose()) {
        window->PollEvents();
        device->Clear(ClearFlagsRHI::All);

        {
            DrawArrays dp(DrawMode::TRIANGLES, 0, 6, vao, programID);
            drawExec->AddParamAndMark(DrawParams{dp});
            queue.Execute();
        }

        {
            SwapChainPresentParams p(1, IntervalType::FIFO);
            swapChainExec->AddParamAndMark(SwapChainParams{p});
            queue.Execute();
        }
    }

    {
        BufferDestroyParams p(vbo, BufferType::VBO);
        bufferExec->AddParamAndMark(BufferParams{p});
        queue.Execute();
    }

    {
        SwapChainDestroyParams p(false);
        swapChainExec->AddParamAndMark(SwapChainParams{p});
        queue.Execute();
    }

    for (auto* e : executers->GetIExecuters()) {
        queue.RemoveExecuter(e);
    }

    shader->UncompileShader();
    shader.reset();

    device->Shutdown();
    device.reset();

    window->Destroy();
    window.reset();

    context->Shutdown();
    context.reset();

    return 0;
}