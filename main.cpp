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
#include "CollisionReaction.h"
#include "Friction.h"

#include "DirectionalLight3D.h"
#include "PointLight3D.h"

#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "Canvas.h"
#include "Text.h"
#include "Font.h"
#include "UIRendering.h"

enum class VisualMode {
    NORMAL, WIREFRAME
};

Polyhedron3D* CreateCubeCollider(float size) {
    float h = size * 0.5f;
    std::vector<Vector3> vertices = {
        Vector3(-h, -h, -h), Vector3( h, -h, -h), Vector3( h,  h, -h), Vector3(-h,  h, -h),
        Vector3(-h, -h,  h), Vector3( h, -h,  h), Vector3( h,  h,  h), Vector3(-h,  h,  h)
    };
    
    Polyhedron3D* cube = new Polyhedron3D(vertices);
    
    for(int i = 0; i < 4; i++) {
        cube->AddCommunication(i, (i+1)%4);
        cube->AddCommunication(i+4, ((i+1)%4)+4);
        cube->AddCommunication(i, i+4);
    }
    
    return cube;
}

struct UIState {
    Text* scoreText = nullptr;
    Text* highScoreText = nullptr;
    Text* speedText = nullptr;
    Text* multiplierText = nullptr;
    int score = 0;
    int highScore = 0;
    float fallSpeed = 0;
    int multiplier = 1;
};

UIState g_uiState;

void SetupUI(UIRendering* uiRenderer, ResourceManager& resources) {
    Font::InitFreeType();
    resources.LoadResource("Assets/ui/arial.ttf", ResourceType::Font);
    Font* arialFont = resources.GetResourceAs<Font>("Assets/ui/arial.ttf");
    
    if (!arialFont) return;
    
    Canvas* canvas = new Canvas(1920.0f, 1080.0f);
    
    GameObject* scoreGO = new GameObject();
    g_uiState.scoreText = scoreGO->AddComponent<Text>(*arialFont, "Score: 0");
    g_uiState.scoreText->SetLayer(0);
    g_uiState.scoreText->rectTransform->SetAnchorMin(Vector2(0, 1));
    g_uiState.scoreText->rectTransform->SetAnchorMax(Vector2(0, 1));
    g_uiState.scoreText->rectTransform->SetAnchoredPosition(Vector2(20, -50));
    g_uiState.scoreText->SetColor(glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));
    g_uiState.scoreText->SetFontSize(32);
    canvas->AddUIElement(g_uiState.scoreText);
    
    GameObject* highScoreGO = new GameObject();
    g_uiState.highScoreText = highScoreGO->AddComponent<Text>(*arialFont, "Best: 0");
    g_uiState.highScoreText->SetLayer(0);
    g_uiState.highScoreText->rectTransform->SetAnchorMin(Vector2(0, 1));
    g_uiState.highScoreText->rectTransform->SetAnchorMax(Vector2(0, 1));
    g_uiState.highScoreText->rectTransform->SetAnchoredPosition(Vector2(20, -100));
    g_uiState.highScoreText->SetColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    g_uiState.highScoreText->SetFontSize(28);
    canvas->AddUIElement(g_uiState.highScoreText);
    
    GameObject* speedGO = new GameObject();
    g_uiState.speedText = speedGO->AddComponent<Text>(*arialFont, "Speed: 0");
    g_uiState.speedText->SetLayer(0);
    g_uiState.speedText->rectTransform->SetAnchorMin(Vector2(0, 1));
    g_uiState.speedText->rectTransform->SetAnchorMax(Vector2(0, 1));
    g_uiState.speedText->rectTransform->SetAnchoredPosition(Vector2(20, -150));
    g_uiState.speedText->SetColor(glm::vec4(0.5f, 0.8f, 1.0f, 1.0f));
    g_uiState.speedText->SetFontSize(28);
    canvas->AddUIElement(g_uiState.speedText);
    
    GameObject* multiplierGO = new GameObject();
    g_uiState.multiplierText = multiplierGO->AddComponent<Text>(*arialFont, "x1");
    g_uiState.multiplierText->SetLayer(0);
    g_uiState.multiplierText->rectTransform->SetAnchorMin(Vector2(0, 1));
    g_uiState.multiplierText->rectTransform->SetAnchorMax(Vector2(0, 1));
    g_uiState.multiplierText->rectTransform->SetAnchoredPosition(Vector2(20, -200));
    g_uiState.multiplierText->SetColor(glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
    g_uiState.multiplierText->SetFontSize(28);
    canvas->AddUIElement(g_uiState.multiplierText);
    
    GameObject* controlsGO = new GameObject();
    Text* controlsText = controlsGO->AddComponent<Text>(*arialFont, "WASD - Move | Space - Jump | F1/F2 - Visual");
    controlsText->SetLayer(0);
    controlsText->rectTransform->SetAnchorMin(Vector2(1, 0));
    controlsText->rectTransform->SetAnchorMax(Vector2(1, 0));
    controlsText->rectTransform->SetAnchoredPosition(Vector2(-20, 20));
    controlsText->SetColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    controlsText->SetFontSize(18);
    canvas->AddUIElement(controlsText);
    
    uiRenderer->RegisterRenderComponent(canvas);
}

void UpdateUI() {
    std::stringstream ss;
    ss << "Score: " << g_uiState.score;
    g_uiState.scoreText->SetText(ss.str());
    
    if (g_uiState.score > g_uiState.highScore) {
        g_uiState.highScore = g_uiState.score;
    }
    
    std::stringstream hs;
    hs << "Best: " << g_uiState.highScore;
    g_uiState.highScoreText->SetText(hs.str());
    
    std::stringstream sp;
    sp << "Speed: " << std::fixed << std::setprecision(1) << g_uiState.fallSpeed;
    g_uiState.speedText->SetText(sp.str());
    
    std::stringstream mult;
    mult << "x" << g_uiState.multiplier;
    g_uiState.multiplierText->SetText(mult.str());
}

int main() {
    srand(time(nullptr));
    Time::Initialize();
    
    float clearColor[4] = {0.05f, 0.05f, 0.1f, 1.0f};
    Window* window = new Window(1600, 900, clearColor, "BEAR ENGINE - DROPPER");
    if (!window->Initialize()) return -1;

    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 12, 22));
    graphics.SetCamera(camera);

    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    ResourceManager resources;
    resources.LoadResource("Assets/models/cube.obj", ResourceType::Model);
    resources.LoadResource("Assets/models/platform.obj", ResourceType::Model);
    resources.LoadResource("Assets/ui/arial.ttf", ResourceType::Font);

    Model* cubeModel = resources.GetResourceAs<Model>("Assets/models/cube.obj");
    if (!cubeModel || !cubeModel->IsLoaded()) {
        std::cout << "Failed to load model!" << std::endl;
        return -1;
    }
    
    PhysicsWorld physicsWorld;
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    UIRendering* uiRenderer = graphics.AddRender<UIRendering>();
    
    SetupUI(uiRenderer, resources);
    
    float green[3] = {0.2f, 1.0f, 0.2f};
    float blue[3] = {0.2f, 0.2f, 1.0f};
    float red[3] = {1.0f, 0.2f, 0.2f};
    float gold[3] = {1.0f, 0.8f, 0.0f};
    
    DirectionalLight3D* dirLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -2, -1).normalized(), Vector3(1, 1, 1), 0.8f
    );
    dirLight->SetShadowArea(80.0f);
    dirLight->SetShadowPlanes(1.0f, 150.0f);
    
    std::vector<PointLight3D*> dynamicLights;
    for (int i = 0; i < 4; i++) {
        PointLight3D* light = renderer->AddLight<PointLight3D>(
            Vector3(i * 8 - 12, 5, i * 6 - 9),
            Vector3(0.3f + i * 0.2f, 0.4f, 0.7f),
            1.5f, 20.0f
        );
        dynamicLights.push_back(light);
    }
    
    GameObject* player = new GameObject();
    Transform3D* playerTransform = player->AddComponent<Transform3D>();
    playerTransform->position = Vector3(0, 10, 0);
    playerTransform->scale = Vector3(0.7f, 1.0f, 0.7f);
    
    Polyhedron3D* playerCollider = CreateCubeCollider(0.9f);
    playerCollider->SetTag("Player");
    player->AddComponent(playerCollider);
    
    PhysicalBody* playerBody = player->AddComponent<PhysicalBody>(playerCollider, 1.5f);
    playerBody->AddFeature<Gravity>();
    playerBody->AddFeature<Friction>(0.3f);
    
    auto* playerImpulse = playerBody->AddFeature<ImpulseModule>();
    playerImpulse->SetMode(ImpulseModuleMode::ARCADY);
    playerImpulse->SetMaxSpeed(14.0f);
    playerImpulse->SetDamping(0.92f);
    
    ModelComponent* playerModel = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", green);
    player->AddComponent(playerModel);
    renderer->RegisterRenderComponent(playerModel);
    physicsWorld.AddBody(playerBody);
    
    GameObject* killPlane = new GameObject();
    Transform3D* killTrans = killPlane->AddComponent<Transform3D>();
    killTrans->position = Vector3(0, -12, 0);
    killTrans->scale = Vector3(100, 0.5f, 100);
    
    Polyhedron3D* killCollider = CreateCubeCollider(1.0f);
    killCollider->SetTag("KillPlane");
    killPlane->AddComponent(killCollider);
    
    PhysicalBody* killBody = killPlane->AddComponent<PhysicalBody>(killCollider, 0.0f);
    physicsWorld.AddBody(killBody);
    
    std::vector<GameObject*> platforms;
    std::vector<GameObject*> obstacles;
    std::vector<GameObject*> collectibles;
    
    for(int i = 0; i < 35; i++) {
        float angle = i * 0.8f;
        float radius = 8.0f + i * 0.3f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float y = 2.0f + i * 1.2f;
        
        GameObject* plat = new GameObject();
        Transform3D* platTrans = plat->AddComponent<Transform3D>();
        platTrans->position = Vector3(x, y, z);
        platTrans->scale = Vector3(2.8f, 0.3f, 2.8f);
        
        Polyhedron3D* platCollider = CreateCubeCollider(1.0f);
        platCollider->SetTag("Platform");
        plat->AddComponent(platCollider);
        
        PhysicalBody* platBody = plat->AddComponent<PhysicalBody>(platCollider, 0.0f);
        
        float brightness = 0.3f + (y / 40.0f);
        float color[3] = {brightness * 0.5f, brightness * 0.4f, brightness * 0.9f};
        ModelComponent* platModel = new ModelComponent(resources, *renderer, "Assets/models/platform.obj", color);
        plat->AddComponent(platModel);
        renderer->RegisterRenderComponent(platModel);
        physicsWorld.AddBody(platBody);
        platforms.push_back(plat);
        
        if (i % 3 == 0 && i > 2) {
            for(int j = -1; j <= 1; j++) {
                if (j == 0) continue;
                GameObject* obstacle = new GameObject();
                Transform3D* obsTrans = obstacle->AddComponent<Transform3D>();
                obsTrans->position = Vector3(x + j * 1.8f, y + 0.8f, z + (rand() % 3 - 1) * 1.5f);
                obsTrans->scale = Vector3(0.8f, 0.8f, 0.8f);
                
                Polyhedron3D* obsCollider = CreateCubeCollider(0.8f);
                obsCollider->SetTag("Obstacle");
                obstacle->AddComponent(obsCollider);
                
                PhysicalBody* obsBody = obstacle->AddComponent<PhysicalBody>(obsCollider, 0.0f);
                
                ModelComponent* obsModel = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", red);
                obstacle->AddComponent(obsModel);
                renderer->RegisterRenderComponent(obsModel);
                physicsWorld.AddBody(obsBody);
                obstacles.push_back(obstacle);
            }
        }
        
        for(int j = -1; j <= 1; j++) {
            if (rand() % 3 == 0) {
                GameObject* coin = new GameObject();
                Transform3D* coinTrans = coin->AddComponent<Transform3D>();
                coinTrans->position = Vector3(x + j * 1.2f, y + 0.6f, z + (rand() % 3 - 1) * 1.2f);
                coinTrans->scale = Vector3(0.4f, 0.4f, 0.4f);
                
                Polyhedron3D* coinCollider = CreateCubeCollider(0.4f);
                coinCollider->SetTag("Collectible");
                coin->AddComponent(coinCollider);
                
                PhysicalBody* coinBody = coin->AddComponent<PhysicalBody>(coinCollider, 0.0f);
                
                ModelComponent* coinModel = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", gold);
                coin->AddComponent(coinModel);
                renderer->RegisterRenderComponent(coinModel);
                physicsWorld.AddBody(coinBody);
                collectibles.push_back(coin);
            }
        }
    }
    
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    colliderManager->AddCollider(playerCollider);
    colliderManager->AddCollider(killCollider);
    for(auto* plat : platforms) {
        colliderManager->AddCollider(plat->GetComponentOfType<BaseCollider>());
    }
    for(auto* obs : obstacles) {
        colliderManager->AddCollider(obs->GetComponentOfType<BaseCollider>());
    }
    for(auto* coin : collectibles) {
        colliderManager->AddCollider(coin->GetComponentOfType<BaseCollider>());
    }
    
    physicsWorld.Start();
    
    g_uiState.score = 0;
    g_uiState.highScore = 0;
    g_uiState.fallSpeed = 0;
    g_uiState.multiplier = 1;
    
    int combo = 0;
    float lastCollectTime = 0;
    float time = 0;
    
    playerCollider->SubscribeToCollision(CollisionEvent::State::ENTER,
        [&](BaseCollider* self, BaseCollider* other) {
            if (other->GetTag() == "Collectible") {
                combo++;
                if (combo > 5) g_uiState.multiplier = 3;
                else if (combo > 2) g_uiState.multiplier = 2;
                else g_uiState.multiplier = 1;
                
                g_uiState.score += 10 * g_uiState.multiplier;
                lastCollectTime = time;
                
                for (auto it = collectibles.begin(); it != collectibles.end(); ++it) {
                    if ((*it)->GetComponentOfType<BaseCollider>() == other) {
                        physicsWorld.RemoveBody((*it)->GetComponentOfType<PhysicalBody>());
                        (*it)->Destroy();
                        collectibles.erase(it);
                        break;
                    }
                }
            }
            else if (other->GetTag() == "Obstacle") {
                combo = 0;
                g_uiState.multiplier = 1;
                g_uiState.score = std::max(0, g_uiState.score - 25);
                std::cout << "Удар! -25 очков! Счёт: " << g_uiState.score << std::endl;
            }
        });
    
    playerCollider->SubscribeToCollision(CollisionEvent::State::ENTER,
        [&](BaseCollider* self, BaseCollider* other) {
            if (other->GetTag() == "KillPlane") {
                combo = 0;
                g_uiState.multiplier = 1;
                g_uiState.score = std::max(0, g_uiState.score / 2);
                playerTransform->position = Vector3(0, 10, 0);
                if (playerImpulse) {
                    playerImpulse->SetVelocity(Vector3(0, 0, 0));
                }
                std::cout << "Падение! Потеряно половина очков! Счёт: " << g_uiState.score << std::endl;
            }
        });
    
    VisualMode currentMode = VisualMode::NORMAL;
    
    std::cout << "\n=== DROPPER ===\n";
    std::cout << "Собирай золотые монеты!\n";
    std::cout << "Комбо x2 (3 монеты) | x3 (6 монет)\n";
    std::cout << "Избегай красных препятствий! -25 очков\n";
    std::cout << "Не падай вниз! Потеря половины очков\n";
    std::cout << "WASD - движение, Space - прыжок\n";
    
    bool isGrounded = false;
    
    while (!window->ShouldClose()) {
        Time::Tick();
        float deltaTime = Time::DeltaTime();
        time += deltaTime;
        
        input.Update();
        
        auto* impulse = playerBody->GetFeatureOfType<ImpulseModule>();
        
        Vector3 moveDir(0, 0, 0);
        float moveSpeed = 14.0f;
        
        if (input.GetKey(Keys::W)) moveDir.z -= 1.0f;
        if (input.GetKey(Keys::S)) moveDir.z += 1.0f;
        if (input.GetKey(Keys::A)) moveDir.x -= 1.0f;
        if (input.GetKey(Keys::D)) moveDir.x += 1.0f;
        
        if (moveDir.magnitude() > 0 && impulse) {
            impulse->AddForce(moveDir.normalized(), 120.0f);
        }
        
        isGrounded = false;
        for (const auto& [other, _] : playerCollider->GetCurrentCollisions()) {
            if (other && other->GetTag() == "Platform") {
                isGrounded = true;
                break;
            }
        }
        
        if (input.GetKeyDown(Keys::Space) && isGrounded && impulse) {
            impulse->AddForce(Vector3(0, 13, 0), 22.0f);
        }
        
        Vector3 velocity = impulse ? impulse->GetVelocity() : Vector3(0, 0, 0);
        g_uiState.fallSpeed = -velocity.y;
        
        if (time - lastCollectTime > 2.0f) {
            combo = 0;
            g_uiState.multiplier = 1;
        }
        
        Vector3 targetPos = playerTransform->position + Vector3(0, 3, 18);
        camera->position = camera->position * 0.97f + targetPos * 0.03f;
        
        for (size_t i = 0; i < dynamicLights.size(); i++) {
            float offset = i * 1.8f;
            Vector3 pos(
                sin(time * 0.5f + offset) * 12.0f,
                6.0f + sin(time * 0.7f + offset) * 4.0f,
                cos(time * 0.5f + offset) * 12.0f
            );
            dynamicLights[i]->SetPosition(pos);
        }
        
        physicsWorld.Update();
        UpdateUI();
        
        if (input.GetKeyDown(Keys::F1)) {
            currentMode = VisualMode::NORMAL;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (input.GetKeyDown(Keys::F2)) {
            currentMode = VisualMode::WIREFRAME;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        
        window->Clear();
        graphics.Update();
        window->SwapBuffers();
        
        if (input.GetKeyDown(Keys::Escape)) break;
    }
    
    return 0;
}