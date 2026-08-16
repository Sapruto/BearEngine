#include "GraphicsManager.h"
#include "Window.h"
#include "Camera3D.h"
#include "Model.h"
#include "ModelComponent.h"
#include "ModelRenderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "InputSystem.h"
#include "Time.h"

#include "Force.h"
#include "ColliderManager.h"
#include "PhysicsWorld.h"
#include "PhysicalBody.h"
#include "Polyhedron3D.h"  
#include "Newtonian.h"
#include "Gravity.h"
#include "Friction.h"

#include "Canvas.h"
#include "Image.h"
#include "Text.h"
#include "Font.h"
#include "Texture.h"
#include "UIRendering.h"
#include "RectTransform.h"

#include "DirectionalLight3D.h"
#include "SimpleModelRenderer.h"

#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const char* MODEL_CUBE = "Assets/models/spheree.obj";
const char* MODEL_PISTOLET = "Assets/models/balistPistolet.obj";
const float SPRING_K = 900.0f;
const float SPRING_DELTA_L = 0.3f;
const float BALL_MASS = 0.7f;
const float GRAVITY_VALUE = 9.8f;
const float BALL_RADIUS = 0.3f;

const Vector3 START_POS = Vector3(-0.5f, 2.0f, 0.4f);

template<typename T>
std::string toString(T val)
{
    std::ostringstream oss;
    oss<< val;
    return oss.str();
}


Polyhedron3D* CreateCubeCollider(float size) {
    float h = size * 0.5f;
    std::vector<Vector3> vertices = {
        Vector3(-h, -h, -h), Vector3( h, -h, -h), Vector3( h,  h, -h), Vector3(-h,  h, -h),
        Vector3(-h, -h,  h), Vector3( h, -h,  h), Vector3( h,  h,  h), Vector3(-h,  h,  h)
    };
    Polyhedron3D* cube = new Polyhedron3D(vertices);
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            float dx = vertices[i].x - vertices[j].x;
            float dy = vertices[i].y - vertices[j].y;
            float dz = vertices[i].z - vertices[j].z;
            float dist2 = dx*dx + dy*dy + dz*dz;
            if (dist2 < 1.1f && dist2 > 0.9f) {
                cube->AddCommunication(i, j);
            }
        }
    }
    return cube;
}

Polyhedron3D* CreateSphereCollider(float radius, int segments = 12) {
    std::vector<Vector3> vertices;
    for (int i = 0; i <= segments; i++) {
        float theta = i * M_PI / segments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        for (int j = 0; j <= segments; j++) {
            float phi = j * 2 * M_PI / segments;
            float x = radius * sinTheta * cos(phi);
            float y = radius * cosTheta;
            float z = radius * sinTheta * sin(phi);
            vertices.push_back(Vector3(x, y, z));
        }
    }
    Polyhedron3D* sphere = new Polyhedron3D(vertices);
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int idx = i * (segments + 1) + j;
            sphere->AddCommunication(idx, idx + 1);
            sphere->AddCommunication(idx, idx + (segments + 1));
        }
    }
    return sphere;
}

float ballColor[3] = {1.0f, 0.5f, 0.0f};
float grayColor[3] = {0.5f, 0.5f, 0.5f};
float gunColor[3] = {0.3f, 0.3f, 0.3f};

class Ball {
public:
    GameObject* obj;
    Transform3D* transform;
    PhysicalBody* body;
    ImpulseModule* impulse;
    Gravity* gravity;
    
    bool isPhysicsActive = false;
    Vector3 startPos;
    
    Ball(Scene* scene, ResourceManager* resources, ModelRenderer* renderer, 
         PhysicsWorld* world, ColliderManager* collMgr, 
         const Vector3& pos, float radius) {
        
        startPos = pos;
        
        obj = scene->CreateGameObject();
        obj->SetName("Ball");
        
        transform = obj->AddComponent<Transform3D>();
        transform->SetLocalPosition(pos);
        transform->SetLocalScale(Vector3(radius, radius, radius));
        
        Polyhedron3D* collider = CreateSphereCollider(radius, 10);
        collider->SetTag("Ball");
        obj->AddComponent(collider);
        
        body = obj->AddComponent<PhysicalBody>(collider, BALL_MASS);
        body->SetPhysicWorld(world);
        
        impulse = body->AddFeature<ImpulseModule>();
        impulse->SetMode(ImpulseModuleMode::REALISTIC);
        impulse->SetDamping(0.99f);
        
        gravity = body->AddFeature<Gravity>();
        gravity->SetGravitation(0.0f);
        
        body->AddFeature<Friction>(0.1f);
        
        ModelComponent* model = new ModelComponent(*resources, *renderer, MODEL_CUBE, ballColor);
        obj->AddComponent(model);
        renderer->RegisterRenderComponent(model);
        
        world->AddBody(body);
        collMgr->AddCollider(collider);
    }
    
    void Fire(float speed) {
        isPhysicsActive = true;
        gravity->SetGravitation(GRAVITY_VALUE);
        impulse->SetVelocity(Vector3(0, speed, 0));
    }
    
    void Reset() {
        isPhysicsActive = false;
        gravity->SetGravitation(0.0f);
        impulse->SetVelocity(Vector3::Zero);
        transform->SetLocalPosition(startPos);
    }
    
    void UpdatePhysics() {
        if (!isPhysicsActive) {
            impulse->SetVelocity(Vector3::Zero);
            transform->SetLocalPosition(startPos);
        }
    }
};


class PhysicsStats {
private:
    Text* V;
    Text* h;

public:
    void InitUI(UIRendering* uiRenderer, ResourceManager& resources, Scene& scene) {
        /*Font::InitFreeType();

        resources.LoadResource("Assets/ui/icon.png", ResourceType::Texture);
        resources.LoadResource("Assets/ui/arial.ttf", ResourceType::Font);
        
        Texture* uiTexture = resources.GetResourceAs<Texture>("Assets/ui/icon.png");
        Font* arialFont = resources.GetResourceAs<Font>("Assets/ui/arial.ttf");
        
        if (!uiTexture || !arialFont) return;

        Canvas* uiCanvas = new Canvas(1920.0f, 1080.0f);

        GameObject* bgGO = scene.CreateGameObject();
        Image* background = bgGO->AddComponent<Image>();
        background->SetTexture(uiTexture);
        background->SetLayer(0);
        background->rectTransform->SetAnchorMin(Vector2(0, 0));
        background->rectTransform->SetAnchorMax(Vector2(0, 0));
        background->rectTransform->SetAnchoredPosition(Vector2(0, 0));
        background->rectTransform->SetSizeDelta(Vector2(920, 800));
        background->SetColor(glm::vec4(0.2f, 0.2f, 0.3f, 1.0f));
        uiCanvas->AddUIElement(background);

        GameObject* titleGO = scene.CreateGameObject();
        V = titleGO->AddComponent<Text>(*arialFont, std::to_string(0));
        V->SetLayer(3);
        V->rectTransform->SetAnchorMin(Vector2(0, 0));
        V->rectTransform->SetAnchorMax(Vector2(0, 0));
        V->rectTransform->SetAnchoredPosition(Vector2(400, 50));
        V->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        uiCanvas->AddUIElement(V);

        GameObject* uiGO = scene.CreateGameObject();
        h = uiGO->AddComponent<Text>(*arialFont, "0");
        h->SetLayer(3);
        h->rectTransform->SetAnchorMin(Vector2(0, 0));
        h->rectTransform->SetAnchorMax(Vector2(0, 0));
        h->rectTransform->SetAnchoredPosition(Vector2(400, 100));
        h->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        uiCanvas->AddUIElement(h);

        h->rectTransform->SetSizeDelta(Vector2(800, 100));

        uiRenderer->RegisterRenderComponent(uiCanvas);*/
    }

    void UpdateUI(int iV, int ih) {
        /*std::cout << "[FIRE] pos y = " << ih << " \n";
        std::cout << "[FIRE] v = " << iV << " m/s\n";

        V->SetText(toString<float>(iV));
        h->SetText(toString<float>(ih));*/
    }
};

int main() {
    srand(time(nullptr));
    Time::Initialize();
    
    float clearColor[4] = {0.2f, 0.3f, 0.4f, 1.0f};
    Window* window = new Window(1200, 800, clearColor, "BEAR ENGINE - Ballistic Gun");
    if (!window->Initialize()) return -1;
    
    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 3, 10));
    graphics.SetCamera(camera);
    
    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    renderer->RegisterModelFeatureRenderer(new SimpleModelRenderer());

    UIRendering* uiRenderer = graphics.AddRender<UIRendering>();
    
    PhysicsWorld physicsWorld;
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    Scene mainScene("MainScene");
    ResourceManager* resources = mainScene.GetResourceManager();

    PhysicsStats stats;
    stats.InitUI(uiRenderer, *resources, mainScene);
    
    resources->LoadResource(MODEL_CUBE, ResourceType::Model);
    resources->LoadResource(MODEL_PISTOLET, ResourceType::Model);
    
    DirectionalLight3D* dirLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -2, -1).normalized(), Vector3(1, 1, 1), 0.8f);
    dirLight->SetShadowArea(30.0f);
    
    GameObject* ground = mainScene.CreateGameObject();
    Transform3D* groundT = ground->AddComponent<Transform3D>();
    groundT->SetLocalPosition(Vector3(0, -0.8f, 0));
    groundT->SetLocalScale(Vector3(20, 0.2f, 20));
    
    Polyhedron3D* groundCol = CreateCubeCollider(1.0f);
    groundCol->SetTag("Ground");
    ground->AddComponent(groundCol);
    
    PhysicalBody* groundBody = ground->AddComponent<PhysicalBody>(groundCol, 0.0f);
    groundBody->SetPhysicWorld(&physicsWorld);
    
    ModelComponent* groundModel = new ModelComponent(*resources, *renderer, MODEL_CUBE, grayColor);
    ground->AddComponent(groundModel);
    renderer->RegisterRenderComponent(groundModel);
    physicsWorld.AddBody(groundBody);
    colliderManager->AddCollider(groundCol);
    
    Ball ball(&mainScene, resources, renderer, &physicsWorld, colliderManager.get(), START_POS, BALL_RADIUS);
    
    GameObject* gun = mainScene.CreateGameObject();
    Transform3D* gunTransform = gun->AddComponent<Transform3D>();
    gunTransform->SetLocalPosition(Vector3(0, 0.2f, 0.5f));
    gunTransform->SetLocalScale(Vector3(1.0f, 1.0f, 1.0f));
    gunTransform->SetLocalRotation(Vector3(0.0f, 0.0f, 90.0f));
    ModelComponent* gunModel = new ModelComponent(*resources, *renderer, MODEL_PISTOLET, gunColor);
    gun->AddComponent(gunModel);
    renderer->RegisterRenderComponent(gunModel);
    
    float shootSpeed = SPRING_DELTA_L * sqrt(SPRING_K / BALL_MASS);
    
    physicsWorld.Start();
    mainScene.InitializeScene();
    mainScene.StartScene();
    
    std::cout << "\n========== BALLISTIC GUN ==========\n";
    std::cout << "k = " << SPRING_K << " N/m\n";
    std::cout << "dx = " << SPRING_DELTA_L << " m\n";
    std::cout << "m = " << BALL_MASS << " kg\n";
    std::cout << "v = " << shootSpeed << " m/s\n";
    std::cout << "\nCONTROLS:\n";
    std::cout << "  R - charge\n";
    std::cout << "  S - FIRE!\n";
    std::cout << "==================================\n\n";
    
    bool isCharged = false;

    bool isEnterInfo = false;

    float camAngle = 0;
    float camDist = 10;
    float camHeight = 4;
    
    while (!window->ShouldClose() && !input.GetKeyDown(Keys::Escape)) {
        Time::Tick();
        input.Update();
        float dtFrame = Time::DeltaTime();
        
        if (input.GetKey(Keys::Left)) camAngle -= 2.0f * dtFrame;
        if (input.GetKey(Keys::Right)) camAngle += 2.0f * dtFrame;
        if (input.GetKey(Keys::Up)) camHeight += 2.0f * dtFrame;
        if (input.GetKey(Keys::Down)) camHeight -= 2.0f * dtFrame;
        
        camera->GetPosition().x = sin(camAngle) * camDist;
        camera->GetPosition().z = cos(camAngle) * camDist;
        camera->GetPosition().y = camHeight;
        
        Vector3 ballPos = ball.transform->GetLocalPosition();
        
        if (input.GetKey(Keys::R) && !isCharged && !ball.isPhysicsActive) {
            isCharged = true;
        }
        
        if (input.GetKey(Keys::S) && isCharged) {
            ball.Fire(shootSpeed);
            isCharged = false;
            isEnterInfo = true;
        }
        
        if (ball.isPhysicsActive && (ballPos.y < -0.8f || ballPos.y > 20.0f)) {
            isEnterInfo = false;
            ball.Reset();
            std::cout << "[RESET]\n";
        }

        if (isEnterInfo) {
            std::cout << "[FIRE] pos y = " << ball.transform->GetPosition().y << " \n";
            std::cout << "[FIRE] v = " << shootSpeed / 2 << " m/s\n";
            //stats.UpdateUI(shootSpeed, ball.transform->GetPosition().y);
        }
        
        physicsWorld.Update();
        ball.UpdatePhysics();
        mainScene.UpdateScene();
        
        window->Clear();
        graphics.Update();
        window->SwapBuffers();
    }
    
    mainScene.DestroyScene();
    
    return 0;
}