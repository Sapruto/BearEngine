#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cstdint>

#include "RHI/Fabric/FabricTypes.h"
#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/RuntimeRHI.h"
#include "RHI/Base/Contracts/SwapChainExecutorParams.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"

#include "Renderer/RenderBackendAllias.h"
#include "Renderer/3D/Scene/RendererScene3D.h"
#include "Renderer/3D/Passes/DefaultPass.h"
#include "Renderer/3D/Materials/DefaultMaterial.h"
#include "Renderer/3D/Settings/Cameras/Camera3D.h"
#include "Renderer/3D/Settings/Lights/DirectionalLight3D.h"
#include "Renderer/3D/Mesh/Renderable3D.h"
#include "Renderer/3D/Mesh/Mesh.h"

#include "Shared/Resource/ResourceManager.h"

#include "World/GameObject.h"
#include "World/Components/Component.h"
#include "Math/Transform/Transform3D.h"
#include "Math/Quaternion.h"

using namespace RHI;
using namespace RHI::FabricRHI;
using namespace RHI::Executors;
using namespace RHI::Executors::SwapChainExecutor;

constexpr unsigned int kWidth  = 1280;
constexpr unsigned int kHeight = 720;
constexpr float        kFpsIntervalSec = 5.0f;

constexpr int   kCubeCount   = 1000;
constexpr float kCubeRadius  = 0.25f;
constexpr float kBoxHalf     = 12.0f;
constexpr float kSpeedMin    = 1.5f;
constexpr float kSpeedMax    = 4.5f;
constexpr float kRestitution = 0.95f;

static void PrintContextInfo() {
    std::cout << "  context=" << (glfwGetCurrentContext() ? "ok" : "NULL") << "\n";
    std::cout << "  GL_VERSION=" << glGetString(GL_VERSION) << "\n";
    std::cout << "  GL_RENDERER=" << glGetString(GL_RENDERER) << "\n";
    std::cout << "  GLSL=" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    std::cout << "  main thread=" << std::this_thread::get_id() << "\n";
}

static bool SetupSwapChain(ExecutorsOwnerT<Renderer::Base::Backend>* executors,
                            ExecutorQueueRHI* queue) {
    auto* swapChainExec = executors->GetSwapChainExecutor();
    if (!swapChainExec) return false;

    SwapChainCreateParams scp;
    scp.width       = kWidth;
    scp.height      = kHeight;
    scp.bufferCount = 2;
    scp.presentMode = PresentMode::IMMEDIATE;
    scp.colorSpace  = ColorSpace::SRGB_NONLINEAR;

    swapChainExec->AddT<SwapChainCreateParams>(SwapChainCreateParams{ scp })
        .RemoveAfterExecute().MarkProcess().Commit();
    queue->Execute();
    return true;
}

static Mesh* PreloadMesh(ResourceManager& resourceManager, const char* meshPath) {
    resourceManager.LoadResource(meshPath, ResourceType::Mesh);
    auto* mesh = resourceManager.GetResourceAs<Mesh>(meshPath);
    if (!mesh) return nullptr;

    std::cout << "  verts=" << mesh->GetVertices().size()
                << " idx="  << mesh->GetIndices().size()
                << " VBO="  << mesh->GetVBO().id
                << " IBO="  << mesh->GetIBO().id << "\n";
    return mesh;
}

static void SetupInputCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
        if (action != GLFW_PRESS) return;
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
        }
    });
}

struct Particle {
    GameObject*  go        = nullptr;
    Transform3D* transform = nullptr;
    Vector3f     position;
    Vector3f     velocity;
};

class PhysicsWorld {
public:
    void Step(std::vector<Particle>& particles, float dt) {
        const float boxMin = -kBoxHalf + kCubeRadius;
        const float boxMax =  kBoxHalf - kCubeRadius;

        for (auto& p : particles) {
            p.position = p.position + p.velocity * dt;

            if (p.position.x < boxMin) { p.position.x = boxMin; p.velocity.x = -p.velocity.x * kRestitution; }
            if (p.position.x > boxMax) { p.position.x = boxMax; p.velocity.x = -p.velocity.x * kRestitution; }
            if (p.position.y < boxMin) { p.position.y = boxMin; p.velocity.y = -p.velocity.y * kRestitution; }
            if (p.position.y > boxMax) { p.position.y = boxMax; p.velocity.y = -p.velocity.y * kRestitution; }
            if (p.position.z < boxMin) { p.position.z = boxMin; p.velocity.z = -p.velocity.z * kRestitution; }
            if (p.position.z > boxMax) { p.position.z = boxMax; p.velocity.z = -p.velocity.z * kRestitution; }
        }

        ResolveCollisions(particles);

        for (auto& p : particles) {
            if (p.transform) {
                p.transform->SetLocalPosition(p.position);
            }
        }
    }

private:
    struct CellKey {
        int x = 0;
        int y = 0;
        int z = 0;

        bool operator==(const CellKey& o) const {
            return x == o.x && y == o.y && z == o.z;
        }
    };

    struct CellKeyHash {
        size_t operator()(const CellKey& k) const noexcept {
            uint64_t h = 1469598103934665603ull;
            auto mix = [&h](int v) {
                h ^= static_cast<uint64_t>(static_cast<uint32_t>(v));
                h *= 1099511628211ull;
            };
            mix(k.x);
            mix(k.y);
            mix(k.z);
            return static_cast<size_t>(h);
        }
    };

    static CellKey CellOf(const Vector3f& p, float cellSize) {
        return CellKey{
            static_cast<int>(std::floor(p.x / cellSize)),
            static_cast<int>(std::floor(p.y / cellSize)),
            static_cast<int>(std::floor(p.z / cellSize))
        };
    }

    void ResolveCollisions(std::vector<Particle>& particles) {
        const float diameter = 2.0f * kCubeRadius;
        const float cellSize = diameter;

        grid_.clear();
        grid_.reserve(particles.size() * 2);

        for (size_t i = 0; i < particles.size(); ++i) {
            grid_[CellOf(particles[i].position, cellSize)].push_back(i);
        }

        for (size_t i = 0; i < particles.size(); ++i) {
            const CellKey base = CellOf(particles[i].position, cellSize);

            for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
            for (int dz = -1; dz <= 1; ++dz) {
                auto it = grid_.find(CellKey{ base.x + dx, base.y + dy, base.z + dz });
                if (it == grid_.end()) continue;

                for (size_t j : it->second) {
                    if (j <= i) continue;
                    ResolvePair(particles[i], particles[j]);
                }
            }
        }
    }

    static void ResolvePair(Particle& a, Particle& b) {
        Vector3f delta = b.position - a.position;
        const float distSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        const float minDist = 2.0f * kCubeRadius;

        if (distSq >= minDist * minDist) return;
        if (distSq < 1e-8f) {
            a.position.x += 1e-3f;
            b.position.x -= 1e-3f;
            return;
        }

        const float dist = std::sqrt(distSq);
        Vector3f n = delta * (1.0f / dist);

        const float penetration = minDist - dist;
        const Vector3f correction = n * (penetration * 0.5f);
        a.position = a.position - correction;
        b.position = b.position + correction;

        const Vector3f relVel = b.velocity - a.velocity;
        const float vn = relVel.x * n.x + relVel.y * n.y + relVel.z * n.z;
        if (vn > 0.0f) return;

        const float impulse = -(1.0f + kRestitution) * vn * 0.5f;
        a.velocity = a.velocity - n * impulse;
        b.velocity = b.velocity + n * impulse;
    }

    std::unordered_map<CellKey, std::vector<size_t>, CellKeyHash> grid_;
};

static void BuildScene(RendererScene3D& scene,
                        ResourceManager& resourceManager,
                        const char* meshPath,
                        std::vector<Particle>& outParticles,
                        std::vector<GameObject*>& outGOs,
                        GameObject*& outCameraGO,
                        GameObject*& outLightGO) {
    outParticles.clear();
    outGOs.clear();
    outParticles.reserve(kCubeCount);
    outGOs.reserve(kCubeCount);

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> posDist(-kBoxHalf + kCubeRadius, kBoxHalf - kCubeRadius);
    std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> speedDist(kSpeedMin, kSpeedMax);
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);

    for (int i = 0; i < kCubeCount; ++i) {
        auto* cube = new GameObject("Cube" + std::to_string(i));
        cube->SetScene(nullptr);

        auto* tr = cube->AddComponent<Transform3D>();
        const Vector3f pos(posDist(rng), posDist(rng), posDist(rng));
        tr->SetLocalPosition(pos);
        tr->SetLocalScale(Vector3f(kCubeRadius * 2.0f, kCubeRadius * 2.0f, kCubeRadius * 2.0f));

        auto* renderable = cube->AddComponent<Renderable3D>(resourceManager, meshPath);

        auto mat = std::make_unique<DefaultMaterial>();
        mat->SetColor(Vector3f(colorDist(rng), colorDist(rng), colorDist(rng)));
        renderable->AddMaterial(std::move(mat));

        scene.RegisterRenderable(renderable);

        Vector3f dir(dirDist(rng), dirDist(rng), dirDist(rng));
        if (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z < 1e-4f) dir = Vector3f(1.0f, 0.0f, 0.0f);
        dir = dir.normalized();

        Particle p;
        p.go        = cube;
        p.transform = tr;
        p.position  = pos;
        p.velocity  = dir * speedDist(rng);

        outParticles.push_back(p);
        outGOs.push_back(cube);
    }

    outCameraGO = new GameObject("Camera");
    outCameraGO->SetScene(nullptr);

    auto* camTransform = outCameraGO->AddComponent<Transform3D>();
    camTransform->SetLocalPosition(Vector3f(0.0f, 0.0f, kBoxHalf * 3.0f));
    camTransform->SetLocalRotation(Quaternionf::Identity());

    auto* camera = outCameraGO->AddComponent<Camera3D>();
    scene.SetCurrentCamera(camera);

    outLightGO = new GameObject("Light");
    outLightGO->SetScene(nullptr);

    auto* lightTransform = outLightGO->AddComponent<Transform3D>();
    lightTransform->SetLocalPosition(Vector3f(5.0f, 10.0f, 5.0f));

    auto* dirLight = outLightGO->AddComponent<DirectionalLight3D>();
    dirLight->SetDirection(Vector3f(-1.0f, -1.0f, -1.0f).normalized());
    dirLight->SetColor(Vector3f(1.0f, 1.0f, 1.0f));
    dirLight->SetIntensity(1.0f);

    scene.AddLight(dirLight);

    for (auto* go : outGOs) {
        go->Initialize();
        go->Awake();
        go->Start();
    }

    outCameraGO->Initialize();
    outCameraGO->Awake();
    outCameraGO->Start();

    outLightGO->Initialize();
    outLightGO->Awake();
    outLightGO->Start();
}

int main() {
    using Backend = Renderer::Base::Backend;
    using Clock   = std::chrono::steady_clock;

    std::cout << "=== [1] RHI init ===\n";
    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.Initialize(BackendType::OpenGL, "Cube", kWidth, kHeight)) {
        std::cerr << "RHI init failed\n";
        return 1;
    }
    PrintContextInfo();

    auto* window    = rhi.Window();
    auto* queue     = rhi.Queue();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(rhi.Executors());

    std::cout << "\n=== [2] SwapChain ===\n";
    if (!SetupSwapChain(executors, queue)) {
        std::cerr << "  swapchain setup failed\n";
        return 1;
    }
    std::cout << "  swapchain created\n";

    std::cout << "\n=== [3] Mesh preload ===\n";
    ResourceManager resourceManager;
    const char* meshPath = "/home/alex/Projects/BearEngine/Assets/models/cube.obj";
    if (!PreloadMesh(resourceManager, meshPath)) {
        std::cerr << "  MESH LOAD FAILED\n";
        return 1;
    }

    std::cout << "\n=== [4] Scene setup ===\n";
    RendererScene3D scene;
    std::vector<Particle> particles;
    std::vector<GameObject*> cubes;
    GameObject* cameraGO = nullptr;
    GameObject* lightGO  = nullptr;
    BuildScene(scene, resourceManager, meshPath, particles, cubes, cameraGO, lightGO);
    std::cout << "  components initialized (" << particles.size() << " cubes)\n";

    std::cout << "\n=== [5] Register pass + Start ===\n";
    scene.RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());
    scene.Start();
    scene.SetClearColor(0.02f, 0.03f, 0.08f, 1.0f);
    std::cout << "  scene started\n";

    SetupInputCallbacks(glfwGetCurrentContext());

    std::cout << "\n=== [6] Main loop ===\n";
    std::cout << "  ESC or Q to exit\n";

    PhysicsWorld physics;

    auto lastStats = Clock::now();
    auto lastFrame = Clock::now();
    int  frame     = 0;
    int  fpsFrames = 0;

    while (!window->ShouldClose()) {
        window->PollEvents();

        const auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;
        if (dt > 0.05f) dt = 0.05f;
        if (dt <= 0.0f) dt = 1.0f / 60.0f;

        physics.Step(particles, dt);
        for (auto* cube : cubes) {
            cube->Update();
        }
        cameraGO->Update();
        lightGO->Update();

        scene.Update();

        ++frame;
        ++fpsFrames;

        const float elapsed = std::chrono::duration<float>(Clock::now() - lastStats).count();
        if (elapsed >= kFpsIntervalSec) {
            const float fps = static_cast<float>(fpsFrames) / elapsed;
            std::cout << "  FPS: " << fps
                      << "  |  frames: " << frame
                      << "  |  cubes: " << cubes.size() << "\n";
            fpsFrames = 0;
            lastStats = Clock::now();
        }
    }

    std::cout << "\n=== [7] Cleanup ===\n";
    scene.Destroy();

    for (auto* cube : cubes) delete cube;
    delete cameraGO;
    delete lightGO;

    rhi.Shutdown();
    std::cout << "  done\n";
    return 0;
}