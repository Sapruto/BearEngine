#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <random>

#include "RHI/RuntimeRHI.h"
#include "Domains/Renderer/Base/GraphicInstance.h"

#include "World/World.h"
#include "World/Scene/Scene.h"
#include "World/Scene/Theater.h"
#include "World/GameObject.h"

#include "Math/Transform/Transform3D.h"
#include "Math/Quaternion.h"
#include "Math/Vector/Vector4.h"
#include "Math/Vector/Vector3.h"

#include "Domains/Renderer/3D/Scene/RendererScene3D.h"
#include "Domains/Renderer/3D/Passes/DefaultPass.h"
#include "Domains/Renderer/3D/Materials/DefaultMaterial.h"
#include "Domains/Renderer/3D/Settings/Cameras/Camera3D.h"
#include "Domains/Renderer/3D/Settings/Lights/DirectionalLight3D.h"
#include "Domains/Renderer/3D/Mesh/Renderable3D.h"
#include "Domains/Renderer/3D/Mesh/Mesh.h"

#include "Domains/Core/DomainRoot.h"
#include "Domains/Physics/Body/PhysicalBody.h"
#include "Domains/Physics/Core/PhysicDomain.h"

#include "Domains/Physics/Simulations/Core/MassSimulation/MassFeature.h"
#include "Domains/Physics/Simulations/Core/MassSimulation/MassSimulator.h"
#include "Domains/Physics/Simulations/Core/TransformSimulation/TransformFeature.h"
#include "Domains/Physics/Simulations/Core/TransformSimulation/TransformSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/Dynamics/DynamicFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Dynamics/DynamicSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/Kinematics/KinematicFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Kinematics/KinematicSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/Forces/Gravity/GravityFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Forces/Gravity/GravitySimulator.h"

#include "Systems/Resource/ResourceSystem.h"

using namespace Renderer::Base;

constexpr int   kCubeCount      = 1000;
constexpr float kFpsIntervalSec = 5.0f;
constexpr float kCubeSize       = 1.0f;
constexpr float kStartHeight    = 10.0f;
constexpr float kGravity        = -9.81f;
constexpr float kCubeMass       = 1.0f;
constexpr float kResetBelowY    = -50.0f;
constexpr float kSpawnRadius    = 5.0f;

static void SetupPhysic(Physic::PhysicDomain* physic) {
    physic->AddPhysicSimulator<Physic::Transform::TransformSimulator>();
    physic->AddPhysicSimulator<Physic::Mass::MassSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Kinematics::KinematicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Dynamics::DynamicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Forces::Gravity::GravitySimulator>();
}

static void AttachFeaturesAtBody(Physic::Body::PhysicalBody* body) {
    if (!body) return;
    body->UpsertFeature<Physic::Transform::TransformFeature>();
    body->UpsertFeature<Physic::Mass::MassFeature>();
    body->UpsertFeature<Physic::Mechanics::Kinematics::KinematicFeature>();
    body->UpsertFeature<Physic::Mechanics::Dynamics::DynamicFeature>();
    body->UpsertFeature<Physic::Mechanics::Forces::Gravity::GravityFeature>(
        Vector3f(0.0f, kGravity, 0.0f));
}

struct FallingCube {
    GameObject* go = nullptr;
    Transform3D* tr = nullptr;
    Physic::Body::PhysicalBody* body = nullptr;
};

int main() {
    using Clock = std::chrono::steady_clock;

    GraphicInstance instance;
    if (!instance.Initialize(GraphicConfig())) {
        std::cerr << "RHI init failed\n";
        return 1;
    }
    std::cout << "RHI ok\n";

    World world;

    auto* domainRoot = world.GetDomainRoot();
    auto* renderScene = domainRoot->AddDomainSystem<RendererScene3D>();
    renderScene->SetClearColor(Vector4f(0.02f, 0.03f, 0.08f, 1.0f));
    renderScene->RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    auto* physic = domainRoot->AddDomainSystem<Physic::PhysicDomain>();
    SetupPhysic(physic);
    std::cout << "Domains ok\n";

    auto* theater = world.GetTheater();
    Scene* scene = theater->AddScene(std::make_unique<Scene>("main"));
    theater->ChangeCurrentScene(scene);

    auto* app = world.GetApplication();
    auto* resourceSystem = app->GetSystemOfType<ResourceSystem>();

    const char* meshPath = "Assets/models/cube.obj";
    resourceSystem->LoadResource(meshPath, ResourceType::Mesh);
    auto* mesh = resourceSystem->GetResourceAs<Mesh>(meshPath);
    if (!mesh) {
        std::cerr << "Mesh load failed\n";
        instance.Shutdown();
        return 1;
    }
    std::cout << "Mesh ok: verts=" << mesh->GetVertices().size()
            << " idx=" << mesh->GetIndices().size() << "\n";

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> posDist(-kSpawnRadius, kSpawnRadius);
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);

    std::vector<FallingCube> cubes;
    cubes.reserve(kCubeCount);

    for (int i = 0; i < kCubeCount; ++i) {
        GameObject* cube = scene->CreateGameObject();
        cube->SetName("FallingCube" + std::to_string(i));

        auto* tr = cube->AddComponent<Transform3D>();
        tr->SetLocalPosition(Vector3f(posDist(rng), kStartHeight, posDist(rng)));
        tr->SetLocalScale(Vector3f(kCubeSize, kCubeSize, kCubeSize));

        auto* renderable = cube->AddComponent<Renderable3D>(meshPath);
        auto mat = std::make_unique<DefaultMaterial>();
        mat->SetColor(Vector3f(colorDist(rng), colorDist(rng), colorDist(rng)));
        renderable->AddMaterial(std::move(mat));

        auto* body = cube->AddComponent<Physic::Body::PhysicalBody>();
        AttachFeaturesAtBody(body);
        body->GetFeatureOfType<Physic::Mass::MassFeature>()->SetMass(kCubeMass);

        cubes.push_back({cube, tr, body});
    }

    std::cout << "Cubes ok: count=" << cubes.size() << "\n";

    GameObject* cameraGO = scene->CreateGameObject();
    auto* camT = cameraGO->AddComponent<Transform3D>();
    camT->SetLocalPosition(Vector3f(0.0f, kStartHeight * 0.5f, 25.0f));
    camT->SetLocalRotation(Quaternionf::Identity());
    renderScene->SetCurrentCamera(cameraGO->AddComponent<Camera3D>());

    GameObject* lightGO = scene->CreateGameObject();
    lightGO->AddComponent<Transform3D>()->SetLocalPosition(Vector3f(5.0f, 10.0f, 5.0f));
    auto* dirLight = lightGO->AddComponent<DirectionalLight3D>();
    dirLight->SetDirection(Vector3f(-1.0f, -1.0f, -1.0f).normalized());
    dirLight->SetColor(Vector3f(1.0f, 1.0f, 1.0f));
    dirLight->SetIntensity(1.0f);
    renderScene->AddLight(dirLight);

    glfwSetKeyCallback(glfwGetCurrentContext(), [](GLFWwindow* w, int key, int, int action, int) {
        if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q))
            glfwSetWindowShouldClose(w, GLFW_TRUE);
    });

    world.StartWorld();
    std::cout << "World started\n";

    auto* window = instance.GetWindow();
    auto start     = Clock::now();
    auto lastStats = Clock::now();
    int  frames    = 0;
    int  fpsFrames = 0;

    while (!window->ShouldClose()) {
        instance.BeginFrame();
        window->PollEvents();

        world.UpdateWorld();

        for (auto& c : cubes) {
            if (c.tr->GetLocalPosition().y < kResetBelowY) {
                c.tr->SetLocalPosition(Vector3f(posDist(rng), kStartHeight, posDist(rng)));
                auto* kin = c.body->GetFeatureOfType<Physic::Mechanics::Kinematics::KinematicFeature>();
                if (kin) kin->velocity = Vector3f::Zero();
            }
        }

        ++frames;
        ++fpsFrames;

        const float elapsed = std::chrono::duration<float>(Clock::now() - lastStats).count();
        if (elapsed >= kFpsIntervalSec) {
            std::cout << "[phys] t=" << std::fixed << std::setprecision(2)
                    << std::chrono::duration<float>(Clock::now() - start).count() << "s"
                    << " fps=" << std::setprecision(1) << (fpsFrames / elapsed)
                    << " frames=" << frames
                    << " cubes=" << cubes.size() << "\n";
            fpsFrames = 0;
            lastStats = Clock::now();
        }

        instance.EndFrame();
    }

    world.DestroyWorld();
    instance.Shutdown();
    std::cout << "Done\n";
    return 0;
}