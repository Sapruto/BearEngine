#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <string>
#include <algorithm>
#include <cmath>

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

#include "Domains/Physics/Simulations/Thermodynamics/Thermal/ThermalFeature.h"
#include "Domains/Physics/Simulations/Thermodynamics/Thermal/ThermalSimulator.h"
#include "Domains/Physics/Simulations/Thermodynamics/Thermal/ThermalEvent.h"

#include "Systems/Resource/ResourceSystem.h"
#include "Systems/Time/TimeSystem.h"

using namespace Renderer::Base;

constexpr float kStatsInterval = 1.0f;
constexpr float kCubeSize = 1.0f;
constexpr float kTempMin = 200.0f;
constexpr float kTempMax = 800.0f;
constexpr float kConductivity = 0.3f;

static Vector3f TemperatureToColor(float t) {
    const float n = std::clamp((t - kTempMin) / (kTempMax - kTempMin), 0.0f, 1.0f);
    if (n < 0.5f) {
        const float u = n * 2.0f;
        return Vector3f(0.1f + 0.9f * u, 0.1f + 0.4f * u, 1.0f - 0.9f * u);
    }
    const float u = (n - 0.5f) * 2.0f;
    return Vector3f(1.0f, 0.5f - 0.4f * u, 0.1f * u);
}

static void SetupPhysic(Physic::PhysicDomain* physic) {
    physic->AddPhysicSimulator<Physic::Transform::TransformSimulator>();
    physic->AddPhysicSimulator<Physic::Mass::MassSimulator>();
    physic->AddPhysicSimulator<Physic::Thermodynamics::Thermal::ThermalSimulator>();
}

struct ThermalBody {
    GameObject* go = nullptr;
    Transform3D* tr = nullptr;
    Renderable3D* rend = nullptr;
    Physic::Body::PhysicalBody* body = nullptr;
    Physic::Mass::MassFeature* mass = nullptr;
    Physic::Thermodynamics::Thermal::ThermalFeature* thermal = nullptr;
    DefaultMaterial* mat = nullptr;
};

static ThermalBody CreateCube(Scene& scene,
                            const char* meshPath,
                            const Vector3f& position,
                            float massValue,
                            float temperature,
                            float specificHeat)
{
    ThermalBody c;
    c.go = scene.CreateGameObject();

    c.tr = c.go->AddComponent<Transform3D>();
    c.tr->SetLocalPosition(position);
    c.tr->SetLocalScale(Vector3f(kCubeSize, kCubeSize, kCubeSize));

    c.rend = c.go->AddComponent<Renderable3D>(meshPath);
    auto mat = std::make_unique<DefaultMaterial>();
    mat->SetColor(TemperatureToColor(temperature));
    c.mat = mat.get();
    c.rend->AddMaterial(std::move(mat));

    c.body = c.go->AddComponent<Physic::Body::PhysicalBody>();
    c.body->UpsertFeature<Physic::Transform::TransformFeature>();
    c.body->UpsertFeature<Physic::Mass::MassFeature>();
    c.body->UpsertFeature<Physic::Thermodynamics::Thermal::ThermalFeature>(temperature, specificHeat);

    c.mass = c.body->GetFeatureOfType<Physic::Mass::MassFeature>();
    c.thermal = c.body->GetFeatureOfType<Physic::Thermodynamics::Thermal::ThermalFeature>();

    c.mass->SetMass(massValue);

    if (auto* tf = c.body->GetFeatureOfType<Physic::Transform::TransformFeature>()) {
        tf->position = c.tr->GetLocalPosition();
        tf->scale = c.tr->GetLocalScale();
        tf->rotation = c.tr->GetLocalRotation();
    }

    return c;
}

int main() {
    using Clock = std::chrono::steady_clock;

    GraphicInstance instance;
    if (!instance.Initialize(GraphicConfig())) {
        std::cerr << "RHI init failed\n";
        return 1;
    }

    World world;

    auto* domainRoot = world.GetDomainRoot();
    auto* renderScene = domainRoot->AddDomainSystem<RendererScene3D>();
    renderScene->SetClearColor(Vector4f(0.02f, 0.03f, 0.08f, 1.0f));
    renderScene->RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    auto* physic = domainRoot->AddDomainSystem<Physic::PhysicDomain>();
    SetupPhysic(physic);

    auto* theater = world.GetTheater();
    Scene* scene = theater->AddScene(std::make_unique<Scene>("thermal"));
    theater->ChangeCurrentScene(scene);

    auto* app = world.GetApplication();
    auto* resourceSystem = app->GetSystemOfType<ResourceSystem>();
    auto* timeSystem = app->GetSystemOfType<TimeSystem>();

    const char* meshPath = "Assets/models/cube.obj";
    resourceSystem->LoadResource(meshPath, ResourceType::Mesh);
    auto* mesh = resourceSystem->GetResourceAs<Mesh>(meshPath);
    if (!mesh) {
        std::cerr << "Mesh load failed\n";
        instance.Shutdown();
        return 1;
    }

    ThermalBody a = CreateCube(*scene, meshPath,
                            Vector3f(-0.5f, 0.0f, 0.0f),
                            1.0f, 800.0f, 1.0f);
    ThermalBody b = CreateCube(*scene, meshPath,
                            Vector3f( 0.5f, 0.0f, 0.0f),
                            4.0f, 200.0f, 1.0f);

    GameObject* cameraGO = scene->CreateGameObject();
    auto* camT = cameraGO->AddComponent<Transform3D>();
    camT->SetLocalPosition(Vector3f(0.0f, 1.5f, 6.0f));
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

    auto* window = instance.GetWindow();
    auto start = Clock::now();
    auto lastStats = Clock::now();
    int frames = 0;
    int fpsFrames = 0;

    while (!window->ShouldClose()) {
        instance.BeginFrame();
        window->PollEvents();

        world.UpdateWorld();

        const float dt = timeSystem ? timeSystem->DeltaTime() : (1.0f / 60.0f);
        const float dT = a.thermal->temperature - b.thermal->temperature;
        if (std::abs(dT) > 1e-4f && dt > 0.0f) {
            const float energy = kConductivity * dT * dt;

            auto& bus = physic->GetEventBus();
            bus.AddEvent(std::make_unique<Physic::Thermodynamics::Thermal::ThermalByEnergyEvent>(
                a.body->GetId(), -energy));
            bus.AddEvent(std::make_unique<Physic::Thermodynamics::Thermal::ThermalByEnergyEvent>(
                b.body->GetId(), +energy));
        }

        a.mat->SetColor(TemperatureToColor(a.thermal->temperature));
        b.mat->SetColor(TemperatureToColor(b.thermal->temperature));

        ++frames;
        ++fpsFrames;

        const float elapsed = std::chrono::duration<float>(Clock::now() - lastStats).count();
        if (elapsed >= kStatsInterval) {
            std::cout << "[thermal] t=" << std::fixed << std::setprecision(2)
                    << std::chrono::duration<float>(Clock::now() - start).count() << "s"
                    << " fps=" << std::setprecision(1) << (fpsFrames / elapsed)
                    << " frames=" << frames
                    << " dt=" << std::setprecision(4) << dt << "\n"
                    << "  A m=" << a.mass->GetMass()
                    << " T=" << std::setprecision(2) << a.thermal->temperature
                    << "  B m=" << b.mass->GetMass()
                    << " T=" << b.thermal->temperature << "\n";
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