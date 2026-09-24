#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseContextInstanceRHI.h"
#include "RHI/OpenGL/Core/ContextInstanceGL.h"

#include "RHI/ExecuterQueueRHI.h"
#include "RHI/Base/Models/BufferExecuterParams.h"
#include "RHI/Base/Models/BufferExecuterResult.h"
#include "RHI/Base/Models/DrawExecuteParams.h"
#include "RHI/Base/Models/DrawExecuteResult.h"
#include "RHI/Base/Models/SwapChainExecuteParams.h"
#include "RHI/Base/Models/SwapChainExecuteResult.h"

#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ShaderGL.h"
#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "RHI/OpenGL/Executers/DrawExecuterGL.h"
#include "RHI/OpenGL/Executers/SwapChainExecuterGL.h"

using namespace BufferExecute;
using namespace DrawExecute;
using namespace SwapChainExecute;

#ifndef RHI_SHADER_DIR
#define RHI_SHADER_DIR "shaders"
#endif

static const float kVertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
};

int main() {
    std::cout << "I'm in alive." << std::endl;

    ContextInstanceGL context;
    if (!context.Initialize()) {
        std::cerr << "[RHI] context init failed\n";
        return 1;
    }

    std::cout << "I'm in alive2." << std::endl;

    auto window = std::make_shared<WindowGL>();
    if (!window->Create("RHI Triangle", 800, 600)) {
        std::cerr << "[RHI] window create failed\n";
        return 1;
    }

    std::cout << "I'm in alive3." << std::endl;

    auto device = std::make_shared<OpenGLDevice>(window);
    if (!device->Initialize()) {
        std::cerr << "[RHI] device init failed\n";
        return 1;
    }

    std::cout << "I'm in alive4." << std::endl;

    const std::string vsPath = std::string(RHI_SHADER_DIR) + "/tri.vert";
    const std::string fsPath = std::string(RHI_SHADER_DIR) + "/tri.frag";

    ShaderGL shader(vsPath, fsPath);
    if (!shader.IsReady()) {
        std::cerr << "[RHI] shader compile failed: "
                  << vsPath << ", " << fsPath << "\n";
        return 1;
    }

    std::cout << "I'm in alive5." << std::endl;

    ExecuterQueueRHI queue(device);

    auto* bufferExec = new BufferExecuterGL();
    auto* drawExec = new DrawExecuterGL();
    auto* swapChainExec = new SwapChainExecuterGL();

    bufferExec->SetLayer(0);
    drawExec->SetLayer(1);
    swapChainExec->SetLayer(2);

    queue.AddExecuter(bufferExec);
    queue.AddExecuter(drawExec);
    queue.AddExecuter(swapChainExec);

    unsigned int vbo = 0;
    unsigned int vao = 0;

    std::cout << "I'm in alive6." << std::endl;

    {
        VBOCreateParams p{};
        p.data = kVertices;
        p.size = sizeof(kVertices);
        p.usage = BufferUsage::Static;
        p.isGen = true;
        p.isBind = true;

        int id = bufferExec->AddParam(BufferParams{p});

        bufferExec->SubscribeOnce(id, [&](const std::vector<BufferExecuteResult>& r){
            vbo = std::get<VBOCreateResult>(r[0].data).vboID;
        });

        bufferExec->MarkProcess(true);
        queue.Execute();
    }

    {
        VAOCreateParams p{};
        p.vboID = vbo;
        p.iboID = 0;
        p.isGen = true;
        p.isBind = true;

        int id = bufferExec->AddParam(BufferParams{p});

        bufferExec->SubscribeOnce(id, [&](const std::vector<BufferExecuteResult>& r){
            vao = std::get<VAOCreateResult>(r[0].data).vaoID;
        });

        bufferExec->MarkProcess(true);
        queue.Execute();
    }

    {
        VAOSetAttributeParams p{};
        p.vaoID = vao;
        p.vboID = vbo;
        p.index = 0;
        p.type = AttributeType::Float3;
        p.offset = 0;
        p.stride = 6 * sizeof(float);
        p.isNormalized = false;
        p.isInteger = false;
        p.isEnable = true;
        p.divisor = 0;

        bufferExec->AddParam(BufferParams{p});
        bufferExec->MarkProcess(true);
        queue.Execute();
    }

    {
        VAOSetAttributeParams p{};
        p.vaoID = vao;
        p.vboID = vbo;
        p.index = 1;
        p.type = AttributeType::Float3;
        p.offset = 3 * sizeof(float);
        p.stride = 6 * sizeof(float);
        p.isNormalized = false;
        p.isInteger = false;
        p.isEnable = true;
        p.divisor = 0;

        bufferExec->AddParam(BufferParams{p});
        bufferExec->MarkProcess(true);
        queue.Execute();
    }

    {
        SwapChainCreateParams p{};
        p.width = 800;
        p.height = 600;
        p.swapChainSize = 2;
        p.intervalType = IntervalType::FIFO;
        p.colorSpace = 0.0f;

        swapChainExec->AddParam(SwapChainParams{p});
        swapChainExec->MarkProcess(true);
        queue.Execute();
    }

    device->SetClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    std::cout << "I'm in alive7." << std::endl;
    std::cerr << "vbo=" << vbo << " vao=" << vao << " shader=" << shader.GetID() << std::endl;
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    std::cerr << "viewport: " << vp[0] << " " << vp[1] << " " << vp[2] << " " << vp[3] << std::endl;

    GLint linked = 0;
    glGetProgramiv(shader.GetID(), GL_LINK_STATUS, &linked);
    std::cerr << "shader link status: " << linked << std::endl;

    GLint activeAttribs = 0;
    glGetProgramiv(shader.GetID(), GL_ACTIVE_ATTRIBUTES, &activeAttribs);
    std::cerr << "active attributes: " << activeAttribs << std::endl;

    GLint activeUniforms = 0;
    glGetProgramiv(shader.GetID(), GL_ACTIVE_UNIFORMS, &activeUniforms);
    std::cerr << "active uniforms: " << activeUniforms << std::endl;

    std::cerr << "=== VAO check ===" << std::endl;

    glBindVertexArray(vao);

    GLint vboBound = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboBound);
    std::cerr << "attrib0 VBO binding: " << vboBound << std::endl;

    GLint enabled0 = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled0);
    std::cerr << "attrib0 enabled: " << enabled0 << std::endl;

    GLint size0 = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size0);
    std::cerr << "attrib0 size: " << size0 << std::endl;

    GLint type0 = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type0);
    std::cerr << "attrib0 type: 0x" << std::hex << type0 << std::dec << std::endl;

    GLint stride0 = 0;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride0);
    std::cerr << "attrib0 stride: " << stride0 << std::endl;

    GLint vboBound1 = 0;
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboBound1);
    std::cerr << "attrib1 VBO binding: " << vboBound1 << std::endl;

    GLint enabled1 = 0;
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled1);
    std::cerr << "attrib1 enabled: " << enabled1 << std::endl;

    glBindVertexArray(0);
    while (!window->ShouldClose()) {
        window->PollEvents();

        device->Clear(ClearFlagsRHI::All);

        {
            DrawArrays dp{};
            dp.vao = vao;
            dp.shaderProgram = shader.GetID();
            dp.mode = DrawMode::TRIANGLES;
            dp.first = 0;
            dp.count = 3;

            drawExec->AddParam(DrawParams{dp});
            drawExec->MarkProcess(true);
            queue.Execute();
        }

        {
            SwapChainPresentParams p{};
            p.syncInterval = 1;
            p.intervalType = IntervalType::FIFO;

            swapChainExec->AddParam(SwapChainParams{p});
            swapChainExec->MarkProcess(true);
            queue.Execute();
        }
    }

    {
        BufferDestroyParams p{};
        p.bufferId = vbo;
        p.bufferType = BufferType::VBO;

        bufferExec->AddParam(BufferParams{p});
        bufferExec->MarkProcess(true);
        queue.Execute();
    }

    {
        SwapChainDestroyParams p{};
        p.forceImmediate = false;

        swapChainExec->AddParam(SwapChainParams{p});
        swapChainExec->MarkProcess(true);
        queue.Execute();
    }

    queue.RemoveExecuter(bufferExec);
    queue.RemoveExecuter(drawExec);
    queue.RemoveExecuter(swapChainExec);

    delete bufferExec;
    delete drawExec;
    delete swapChainExec;

    shader.UncompileShader();

    device->Shutdown();
    window->Destroy();
    context.Shutdown();

    return 0;
}