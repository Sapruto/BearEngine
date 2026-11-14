#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <string>
#include <vector>
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
#include "Domains/Physics/Simulations/Mechanics/Dynamics/DynamicFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Dynamics/DynamicSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/Kinematics/KinematicFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Kinematics/KinematicSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/Forces/Gravity/GravityFeature.h"
#include "Domains/Physics/Simulations/Mechanics/Forces/Gravity/GravitySimulator.h"
#include "Domains/Physics/Simulations/Mechanics/ContactResolution/ContactSimulator.h"
#include "Domains/Physics/Simulations/Mechanics/ContactResolution/RestitutionFeature.h"

#include "Domains/Collisions/Core/CollisionDomain.h"
#include "Domains/Collisions/3D/CollisionDetector3D.h"
#include "Domains/Collisions/3D/ShapeBuilder.h"
#include "Domains/Collisions/3D/Polyhedron3D.h"

#include "Systems/Resource/ResourceSystem.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Input/Keys.h"

using namespace Renderer::Base;

constexpr float kGravity       = -9.81f;
constexpr float kCubeSize      = 1.0f;
constexpr float kFallStartY    = 10.0f;
constexpr float kFloorY        = -0.5f;
constexpr float kDynamicMass   = 1.0f;
constexpr float kStaticMass    = 100000.0f;
constexpr float kRestitution   = 0.35f;
constexpr float kStatsInterval = 3.0f;
constexpr float kCamSpeed      = 8.0f;
constexpr float kMouseSens     = 0.0025f;

static Vector3f HslToRgb(float h, float s, float l) {
    auto hue2rgb = [](float p, float q, float t) {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;
        if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f) return q;
        if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    };
    if (s == 0.0f) return Vector3f(l, l, l);
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    return Vector3f(hue2rgb(p, q, h + 1.0f / 3.0f),
                    hue2rgb(p, q, h),
                    hue2rgb(p, q, h - 1.0f / 3.0f));
}

static void SetupPhysic(Physic::PhysicDomain* physic, CollisionDomain* collisionDomain) {
    physic->AddPhysicSimulator<Physic::Transform::TransformSimulator>();
    physic->AddPhysicSimulator<Physic::Mass::MassSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Kinematics::KinematicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Dynamics::DynamicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Forces::Gravity::GravitySimulator>();

    auto* contactSim = physic->AddPhysicSimulator<Physic::Mechanics::Contact::ContactSimulator>();
    if (contactSim) contactSim->SetCollisionDomain(collisionDomain);
}

static void AttachDynamicFeatures(Physic::Body::PhysicalBody* body, float mass) {
    if (!body) return;
    body->UpsertFeature<Physic::Transform::TransformFeature>();
    body->UpsertFeature<Physic::Mass::MassFeature>()->SetMass(mass);
    body->UpsertFeature<Physic::Mechanics::Kinematics::KinematicFeature>();
    body->UpsertFeature<Physic::Mechanics::Dynamics::DynamicFeature>();
    body->UpsertFeature<Physic::Mechanics::Forces::Gravity::GravityFeature>(
        Vector3f(0.0f, kGravity, 0.0f));
    body->UpsertFeature<Physic::Mechanics::Contact::RestitutionFeature>(kRestitution);
}

static void AttachStaticFeatures(Physic::Body::PhysicalBody* body) {
    if (!body) return;
    auto* tf = body->UpsertFeature<Physic::Transform::TransformFeature>();
    tf->isConst = true;
    body->UpsertFeature<Physic::Mass::MassFeature>()->SetMass(kStaticMass);
    body->UpsertFeature<Physic::Mechanics::Kinematics::KinematicFeature>();
    body->UpsertFeature<Physic::Mechanics::Contact::RestitutionFeature>(kRestitution);
}

static void AttachBoxCollider(GameObject* go, CollisionDomain* collisionDomain,
                            float sx, float sy, float sz) {
    ShapeBuilder builder;
    auto box = builder.CreateBox(Vector3f(sx, sy, sz));
    go->TakeComponent<Polyhedron3D>(std::move(box));
    collisionDomain->AddByObject(go);
}

static GameObject* SpawnCube(
    Scene* scene, CollisionDomain* collisionDomain, const char* meshPath,
    const Vector3f& pos, const Vector3f& scale, const Vector3f& color, bool dynamic)
{
    GameObject* go = scene->CreateGameObject();
    auto* tr = go->AddComponent<Transform3D>();
    tr->SetLocalPosition(pos);
    tr->SetLocalScale(scale);

    auto* renderable = go->AddComponent<Renderable3D>(meshPath);
    {
        auto mat = std::make_unique<DefaultMaterial>();
        mat->SetColor(color);
        renderable->AddMaterial(std::move(mat));
    }

    auto* body = go->AddComponent<Physic::Body::PhysicalBody>();
    if (dynamic) {
        AttachDynamicFeatures(body, kDynamicMass);
    } else {
        AttachStaticFeatures(body);
    }
    AttachBoxCollider(go, collisionDomain, scale.x, scale.y, scale.z);
    return go;
}

int main() {
    using Clock = std::chrono::steady_clock;

    GraphicInstance instance;
    GraphicConfig config;
    config.width = 1600;
    config.height = 900;
    if (!instance.Initialize(config)) {
        std::cerr << "[init] RHI init failed\n";
        return 1;
    }
    std::cout << "[init] RHI ok, window " << config.width << "x" << config.height << "\n";

    GLFWwindow* glfwWindow = glfwGetCurrentContext();
    auto& input = InputSystem::GetInstance();
    input.Initialize(glfwWindow);

    World world;
    auto* domainRoot = world.GetDomainRoot();

    auto* renderScene = domainRoot->AddDomainSystem<RendererScene3D>();
    renderScene->SetClearColor(Vector4f(0.03f, 0.04f, 0.09f, 1.0f));
    renderScene->RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    auto* collisionDomain = domainRoot->AddDomainSystem<CollisionDomain>();
    collisionDomain->AddDetector(std::make_unique<CollisionDetector3D>(collisionDomain->GetRegistry()));
    std::cout << "[init] CollisionDomain ok\n";

    auto* physic = domainRoot->AddDomainSystem<Physic::PhysicDomain>();
    SetupPhysic(physic, collisionDomain);
    std::cout << "[init] PhysicDomain ok\n";

    auto* theater = world.GetTheater();
    Scene* scene = theater->AddScene(std::make_unique<Scene>("main"));
    theater->ChangeCurrentScene(scene);

    auto* app = world.GetApplication();
    auto* resourceSystem = app->GetSystemOfType<ResourceSystem>();

    const char* meshPath = "Assets/models/cube.obj";
    resourceSystem->LoadResource(meshPath, ResourceType::Mesh);
    auto* mesh = resourceSystem->GetResourceAs<Mesh>(meshPath);
    if (!mesh) {
        std::cerr << "[init] Mesh load failed: " << meshPath << "\n";
        instance.Shutdown();
        return 1;
    }
    std::cout << "[init] Mesh ok: verts=" << mesh->GetVertices().size()
            << " idx=" << mesh->GetIndices().size() << "\n";

    GameObject* cameraGO = scene->CreateGameObject();
    cameraGO->SetName("MainCamera");
    auto* camT = cameraGO->AddComponent<Transform3D>();
    camT->SetLocalPosition(Vector3f(0.0f, 8.0f, 16.0f));
    camT->SetLocalRotation(Quaternionf::Identity());
    renderScene->SetCurrentCamera(cameraGO->AddComponent<Camera3D>());

    GameObject* sunGO = scene->CreateGameObject();
    sunGO->SetName("Sun");
    sunGO->AddComponent<Transform3D>()->SetLocalPosition(Vector3f(8.0f, 20.0f, 8.0f));
    auto* sun = sunGO->AddComponent<DirectionalLight3D>();
    sun->SetDirection(Vector3f(-0.5f, -1.0f, -0.4f).Normalized());
    sun->SetColor(Vector3f(1.0f, 0.93f, 0.80f));
    sun->SetIntensity(1.25f);
    renderScene->AddLight(sun);

    GameObject* fillGO = scene->CreateGameObject();
    fillGO->SetName("Fill");
    fillGO->AddComponent<Transform3D>()->SetLocalPosition(Vector3f(-8.0f, 10.0f, -6.0f));
    auto* fill = fillGO->AddComponent<DirectionalLight3D>();
    fill->SetDirection(Vector3f(0.7f, -0.8f, 0.6f).Normalized());
    fill->SetColor(Vector3f(0.35f, 0.55f, 1.0f));
    fill->SetIntensity(0.45f);
    renderScene->AddLight(fill);

    GameObject* rimGO = scene->CreateGameObject();
    rimGO->SetName("Rim");
    rimGO->AddComponent<Transform3D>()->SetLocalPosition(Vector3f(0.0f, -4.0f, -8.0f));
    auto* rim = rimGO->AddComponent<DirectionalLight3D>();
    rim->SetDirection(Vector3f(0.2f, 0.6f, -1.0f).Normalized());
    rim->SetColor(Vector3f(0.9f, 0.3f, 0.9f));
    rim->SetIntensity(0.35f);
    renderScene->AddLight(rim);

    SpawnCube(scene, collisionDomain, meshPath,
            Vector3f(0.0f, kFloorY - 0.5f, 0.0f),
            Vector3f(18.0f, 1.0f, 18.0f),
            Vector3f(0.16f, 0.18f, 0.22f), false);

    SpawnCube(scene, collisionDomain, meshPath,
            Vector3f(0.0f, kFloorY + 2.5f, -7.5f),
            Vector3f(1.5f, 5.0f, 1.5f),
            Vector3f(0.25f, 0.15f, 0.35f), false);

    {
        const int base = 4;
        for (int level = 0; level < base; ++level) {
            int count = base - level;
            for (int i = 0; i < count; ++i) {
                float x = (i - (count - 1) * 0.5f) * 1.05f;
                float y = kFloorY + 0.5f + level * 1.05f;
                Vector3f color = HslToRgb(0.55f + level * 0.05f, 0.7f, 0.5f);
                SpawnCube(scene, collisionDomain, meshPath,
                        Vector3f(x, y, -7.5f),
                        Vector3f(1.0f, 1.0f, 1.0f),
                        color, false);
            }
        }
    }

    {
        const int archSegments = 6;
        for (int i = 0; i < archSegments; ++i) {
            float t = static_cast<float>(i) / (archSegments - 1);
            float angle = t * 3.14159265359f;
            float x = std::cos(angle) * 6.0f;
            float y = std::sin(angle) * 4.0f + kFloorY + 0.5f;
            Vector3f color = HslToRgb(0.1f + 0.1f * t, 0.7f, 0.55f);
            SpawnCube(scene, collisionDomain, meshPath,
                    Vector3f(x, y, 6.0f),
                    Vector3f(0.8f, 0.8f, 0.8f),
                    color, false);
        }
    }

    {
        for (int i = 0; i < 10; ++i) {
            float angle = static_cast<float>(i) / 10.0f * 6.2831853f;
            float radius = 9.0f + (i % 3) * 1.5f;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            float y = 6.0f + (i % 4) * 1.2f;
            Vector3f color = HslToRgb(static_cast<float>(i) / 10.0f, 0.8f, 0.6f);
            SpawnCube(scene, collisionDomain, meshPath,
                    Vector3f(x, y, z),
                    Vector3f(0.5f, 0.5f, 0.5f),
                    color, false);
        }
    }

    {
        const int total = 60;
        for (int i = 0; i < total; ++i) {
            float t = static_cast<float>(i) / total;
            float angle = t * 6.2831853f * 3.0f;
            float radius = 3.0f + t * 3.0f;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            float y = kFallStartY + (i % 5) * 1.5f;
            float hue = std::fmod(t * 3.0f, 1.0f);
            Vector3f color = HslToRgb(hue, 0.8f, 0.55f);
            SpawnCube(scene, collisionDomain, meshPath,
                    Vector3f(x, y, z),
                    Vector3f(kCubeSize, kCubeSize, kCubeSize),
                    color, true);
        }
    }

    SpawnCube(scene, collisionDomain, meshPath,
            Vector3f(5.0f, kFloorY + 1.5f, 3.0f),
            Vector3f(2.0f, 3.0f, 2.0f),
            HslToRgb(0.05f, 0.85f, 0.5f), false);
    SpawnCube(scene, collisionDomain, meshPath,
            Vector3f(-5.0f, kFloorY + 0.75f, 3.0f),
            Vector3f(1.5f, 1.5f, 1.5f),
            HslToRgb(0.15f, 0.85f, 0.5f), false);

    world.StartWorld();
    std::cout << "[init] World started\n"
            << "  WASD  — move\n"
            << "  Q/E   — down/up\n"
            << "  RMB   — hold to look around\n"
            << "  Space — pause\n"
            << "  Esc   — quit\n";

    auto* window = instance.GetWindow();
    auto start = Clock::now();
    auto lastStats = Clock::now();
    auto lastFrame = Clock::now();
    int frames = 0, fpsFrames = 0;
    bool paused = false;
    bool mouseCaptured = false;
    double lastMouseX = 0.0, lastMouseY = 0.0;

    while (!window->ShouldClose()) {
        input.BeginUpdate();
        window->PollEvents();

        auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;
        if (dt > 0.1f) dt = 0.1f;

        if (input.GetKeyDown(Keys::Escape)) {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }
        if (input.GetKeyDown(Keys::Space)) {
            paused = !paused;
            std::cout << "[sim] " << (paused ? "paused" : "running") << "\n";
        }

        bool rmbDown = input.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
        if (rmbDown && !mouseCaptured) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwGetCursorPos(glfwWindow, &lastMouseX, &lastMouseY);
            mouseCaptured = true;
        } else if (!rmbDown && mouseCaptured) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            mouseCaptured = false;
        }

        if (mouseCaptured) {
            double mx = 0.0, my = 0.0;
            glfwGetCursorPos(glfwWindow, &mx, &my);
            double dx = mx - lastMouseX;
            double dy = my - lastMouseY;
            lastMouseX = mx;
            lastMouseY = my;

            if (std::fabs(dx) > 0.0001 || std::fabs(dy) > 0.0001) {
                float yawDelta   = static_cast<float>(-dx) * kMouseSens;
                float pitchDelta = static_cast<float>(-dy) * kMouseSens;

                Quaternionf qYaw   = Quaternionf::CreateRotation(yawDelta, Vector3f(0, 1, 0));
                Quaternionf qPitch = Quaternionf::CreateRotation(pitchDelta, Vector3f(1, 0, 0));
                camT->SetLocalRotation((camT->GetLocalRotation() * qYaw * qPitch).Normalized());
            }
        }

        Vector3f move(0.0f, 0.0f, 0.0f);
        if (input.GetKey(Keys::W)) move.z -= 1.0f;
        if (input.GetKey(Keys::S)) move.z += 1.0f;
        if (input.GetKey(Keys::A)) move.x -= 1.0f;
        if (input.GetKey(Keys::D)) move.x += 1.0f;
        if (input.GetKey(Keys::E)) move.y += 1.0f;
        if (input.GetKey(Keys::Q)) move.y -= 1.0f;

        if (move.Magnitude() > 0.001f) {
            move = move.Normalized();
            Quaternionf rot = camT->GetLocalRotation();
            Vector3f forward = rot.RotateVector(Vector3f(0, 0, -1));
            Vector3f right   = rot.RotateVector(Vector3f(1, 0, 0));
            Vector3f up      = rot.RotateVector(Vector3f(0, 1, 0));
            Vector3f worldMove = forward * (-move.z) + right * move.x + up * move.y;
            camT->SetLocalPosition(camT->GetLocalPosition() + worldMove * (kCamSpeed * dt));
        }

        if (!paused) {
            world.UpdateWorld();
        }

        ++frames;
        ++fpsFrames;

        const float elapsed = std::chrono::duration<float>(Clock::now() - lastStats).count();
        if (elapsed >= kStatsInterval) {
            const float t = std::chrono::duration<float>(Clock::now() - start).count();
            size_t contactCount = 0;
            if (auto* det = collisionDomain->GetDetector()) {
                contactCount = det->GetCollisions().size();
            }
            const Vector3f cp = camT->GetLocalPosition();
            std::cout << std::fixed << std::setprecision(2)
                    << "[stats] t=" << t << "s"
                    << " fps=" << std::setprecision(1) << (fpsFrames / elapsed)
                    << " frames=" << frames
                    << " contacts=" << contactCount
                    << " cam=(" << cp.x << "," << cp.y << "," << cp.z << ")"
                    << " rmb=" << (mouseCaptured ? "on" : "off")
                    << " " << (paused ? "[paused]" : "[running]")
                    << "\n";
            fpsFrames = 0;
            lastStats = Clock::now();
        }

        input.EndUpdate();
        instance.EndFrame();
    }

    if (mouseCaptured) {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    world.DestroyWorld();
    instance.Shutdown();
    std::cout << "[done] frames=" << frames << "\n";
    return 0;
}