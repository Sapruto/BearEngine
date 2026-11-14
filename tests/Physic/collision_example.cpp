#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <string>
#include <vector>

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

using namespace Renderer::Base;

constexpr float kGravity      = -9.81f;
constexpr float kCubeSize     = 1.0f;
constexpr float kFallStartY   = 10.0f;
constexpr float kSupportY     = 0.0f;
constexpr float kFallMass     = 1.0f;
constexpr float kSupportMass  = 1000.0f;
constexpr float kRestitution  = 0.2f;
constexpr float kStatsInterval = 5.0f;

static void SetupPhysic(Physic::PhysicDomain* physic, CollisionDomain* collisionDomain) {
    physic->AddPhysicSimulator<Physic::Transform::TransformSimulator>();
    physic->AddPhysicSimulator<Physic::Mass::MassSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Kinematics::KinematicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Dynamics::DynamicSimulator>();
    physic->AddPhysicSimulator<Physic::Mechanics::Forces::Gravity::GravitySimulator>();

    auto* contactSim = physic->AddPhysicSimulator<Physic::Mechanics::Contact::ContactSimulator>();
    if (contactSim) contactSim->SetCollisionDomain(collisionDomain);
}

static void AttachFallingFeatures(Physic::Body::PhysicalBody* body) {
    if (!body) return;
    body->UpsertFeature<Physic::Transform::TransformFeature>();
    body->UpsertFeature<Physic::Mass::MassFeature>()->SetMass(kFallMass);
    body->UpsertFeature<Physic::Mechanics::Kinematics::KinematicFeature>();
    body->UpsertFeature<Physic::Mechanics::Dynamics::DynamicFeature>();
    body->UpsertFeature<Physic::Mechanics::Forces::Gravity::GravityFeature>(
        Vector3f(0.0f, kGravity, 0.0f));
    body->UpsertFeature<Physic::Mechanics::Contact::RestitutionFeature>(kRestitution);
}

static void AttachStaticFeatures(Physic::Body::PhysicalBody* body) {
    if (!body) return;
    body->UpsertFeature<Physic::Transform::TransformFeature>();
    body->UpsertFeature<Physic::Mass::MassFeature>()->SetMass(kSupportMass);
    body->UpsertFeature<Physic::Mechanics::Kinematics::KinematicFeature>();
    body->UpsertFeature<Physic::Mechanics::Dynamics::DynamicFeature>();
    body->UpsertFeature<Physic::Mechanics::Contact::RestitutionFeature>(kRestitution);
}

static Polyhedron3D* AttachBoxCollider(GameObject* go, CollisionDomain* collisionDomain, float size) {
    ShapeBuilder builder;
    auto box = builder.CreateBox(Vector3f(size, size, size));

    auto* collider = go->TakeComponent<Polyhedron3D>(std::move(box));

    collisionDomain->AddByObject(go);
    return collider;
}

int main() {
    using Clock = std::chrono::steady_clock;

    GraphicInstance instance;
    if (!instance.Initialize(GraphicConfig())) {
        std::cerr << "[init] RHI init failed\n";
        return 1;
    }
    std::cout << "[init] RHI ok\n";

    World world;

    auto* domainRoot = world.GetDomainRoot();

    auto* renderScene = domainRoot->AddDomainSystem<RendererScene3D>();
    renderScene->SetClearColor(Vector4f(0.02f, 0.03f, 0.08f, 1.0f));
    renderScene->RegisterRenderer<DefaultPass>(std::make_unique<DefaultPass>());

    auto* collisionDomain = domainRoot->AddDomainSystem<CollisionDomain>();
    collisionDomain->AddDetector(std::make_unique<CollisionDetector3D>(collisionDomain->GetRegistry()));
    std::cout << "[init] CollisionDomain ok, detector="
            << (collisionDomain->GetDetector() ? "yes" : "no") << "\n";

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

    GameObject* falling = scene->CreateGameObject();
    falling->SetName("FallingCube");

    auto* fallTr = falling->AddComponent<Transform3D>();
    fallTr->SetLocalPosition(Vector3f(0.0f, kFallStartY, 0.0f));
    fallTr->SetLocalScale(Vector3f(kCubeSize, kCubeSize, kCubeSize));

    auto* fallRenderable = falling->AddComponent<Renderable3D>(meshPath);
    {
        auto mat = std::make_unique<DefaultMaterial>();
        mat->SetColor(Vector3f(0.2f, 0.8f, 0.2f));
        fallRenderable->AddMaterial(std::move(mat));
    }

    auto* fallBody = falling->AddComponent<Physic::Body::PhysicalBody>();
    AttachFallingFeatures(fallBody);
    AttachBoxCollider(falling, collisionDomain, kCubeSize);
    std::cout << "[init] FallingCube ok, mass=" << kFallMass
            << " startY=" << kFallStartY << "\n";

    GameObject* support = scene->CreateGameObject();
    support->SetName("SupportCube");

    auto* supTr = support->AddComponent<Transform3D>();
    supTr->SetLocalPosition(Vector3f(0.0f, kSupportY, 0.0f));
    supTr->SetLocalScale(Vector3f(kCubeSize, kCubeSize, kCubeSize));

    auto* supRenderable = support->AddComponent<Renderable3D>(meshPath);
    {
        auto mat = std::make_unique<DefaultMaterial>();
        mat->SetColor(Vector3f(0.8f, 0.2f, 0.2f));
        supRenderable->AddMaterial(std::move(mat));
    }

    auto* supBody = support->AddComponent<Physic::Body::PhysicalBody>();
    AttachStaticFeatures(supBody);
    AttachBoxCollider(support, collisionDomain, kCubeSize);
    std::cout << "[init] SupportCube ok, mass=" << kSupportMass
            << " y=" << kSupportY << " (no gravity)\n";

    GameObject* cameraGO = scene->CreateGameObject();
    auto* camT = cameraGO->AddComponent<Transform3D>();
    camT->SetLocalPosition(Vector3f(0.0f, kFallStartY * 0.5f, 12.0f));
    camT->SetLocalRotation(Quaternionf::Identity());
    renderScene->SetCurrentCamera(cameraGO->AddComponent<Camera3D>());

    GameObject* lightGO = scene->CreateGameObject();
    lightGO->AddComponent<Transform3D>()->SetLocalPosition(Vector3f(5.0f, 10.0f, 5.0f));
    auto* dirLight = lightGO->AddComponent<DirectionalLight3D>();
    dirLight->SetDirection(Vector3f(-1.0f, -1.0f, -1.0f).Normalized());
    dirLight->SetColor(Vector3f(1.0f, 1.0f, 1.0f));
    dirLight->SetIntensity(1.0f);
    renderScene->AddLight(dirLight);

    glfwSetKeyCallback(glfwGetCurrentContext(), [](GLFWwindow* w, int key, int, int action, int) {
        if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q))
            glfwSetWindowShouldClose(w, GLFW_TRUE);
    });

    world.StartWorld();
    std::cout << "[init] World started\n";

    auto* window = instance.GetWindow();
    auto start     = Clock::now();
    auto lastStats = Clock::now();
    int  frames    = 0;
    int  fpsFrames = 0;

    while (!window->ShouldClose()) {
        instance.BeginFrame();
        window->PollEvents();

        world.UpdateWorld();

        ++frames;
        ++fpsFrames;

        const float elapsed = std::chrono::duration<float>(Clock::now() - lastStats).count();
        if (elapsed >= kStatsInterval) {
            const float t = std::chrono::duration<float>(Clock::now() - start).count();
            const auto fPos = fallTr->GetLocalPosition();
            const auto sPos = supTr->GetLocalPosition();

            auto* fallKin = fallBody->GetFeatureOfType<Physic::Mechanics::Kinematics::KinematicFeature>();
            const Vector3f fallVel = fallKin ? fallKin->velocity : Vector3f::Zero();

            size_t contactCount = 0;
            if (auto* det = collisionDomain->GetDetector()) {
                contactCount = det->GetCollisions().size();
            }

            std::cout << std::fixed << std::setprecision(3)
                    << "[stats] t=" << std::setprecision(2) << t << "s"
                    << " fps=" << std::setprecision(1) << (fpsFrames / elapsed)
                    << " frames=" << frames
                    << " | fall.pos=(" << fPos.x << "," << fPos.y << "," << fPos.z << ")"
                    << " fall.vel=(" << fallVel.x << "," << fallVel.y << "," << fallVel.z << ")"
                    << " | sup.pos=(" << sPos.x << "," << sPos.y << "," << sPos.z << ")"
                    << " | contacts=" << contactCount
                    << "\n";

            fpsFrames = 0;
            lastStats = Clock::now();
        }

        instance.EndFrame();
    }

    world.DestroyWorld();
    instance.Shutdown();
    std::cout << "[done] frames=" << frames << "\n";
    return 0;
}