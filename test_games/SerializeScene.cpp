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

#include "Canvas.h"
#include "Image.h"
#include "Text.h"
#include "Font.h"
#include "Texture.h"
#include "UIRendering.h"
#include "RectTransform.h"

#include "TransparencyModelRenderer.h"
#include "SimpleModelRenderer.h"
#include "TransparentFeature.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneDeserializer.h"

#include "SceneCreator.h"
#include "SceneSerializer.h"

#include "ComponentLibrary.h"

#include "Component.h"
#include "SerializeFieldComponent.h"


class TestComponent : public Component, public ISerializable {
private:
    FIELD_COMPONENT(Transform3D*, trans);

    FIELD(Vector3, pos);

public:
    void Start() override {}
    void Update() override {}
    void Destroy() override {}

    void SetTransform(Transform3D* transform) {
        trans.GetValue() = transform;
    }

    SERIALIZED_FIELDS(&trans, &pos)
};

enum class VisualMode {
    NORMAL,           
    WIREFRAME,        
    COLLIDERS,        
    LIGHTS            
};

Polyhedron3D* CreateCubeCollider(float size) {
    float h = size * 0.5f;
    
    std::vector<Vector3> vertices = {
        Vector3(-h, -h, -h),  Vector3( h, -h, -h),  Vector3( h,  h, -h),  Vector3(-h,  h, -h),
        Vector3(-h, -h,  h),  Vector3( h, -h,  h),  Vector3( h,  h,  h),  Vector3(-h,  h,  h)
    };
    
    Polyhedron3D* cube = new Polyhedron3D(vertices);
    
    cube->AddCommunication(0, 1);
    cube->AddCommunication(1, 2);
    cube->AddCommunication(2, 3);
    cube->AddCommunication(3, 0);
    
    cube->AddCommunication(4, 5);
    cube->AddCommunication(5, 6);
    cube->AddCommunication(6, 7);
    cube->AddCommunication(7, 4);
    
    cube->AddCommunication(0, 4);
    cube->AddCommunication(1, 5);
    cube->AddCommunication(2, 6);
    cube->AddCommunication(3, 7);
    
    return cube;
}

void InitUI(UIRendering* uiRenderer, ResourceManager& resources, Scene& scene) {
    Font::InitFreeType();

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
    background->rectTransform->SetSizeDelta(Vector2(1920, 1080));
    background->SetColor(glm::vec4(0.2f, 0.2f, 0.3f, 1.0f));
    uiCanvas->AddUIElement(background);

    GameObject* logoGO = scene.CreateGameObject();
    Image* logo = logoGO->AddComponent<Image>();
    logo->SetTexture(uiTexture);
    logo->SetLayer(1);
    logo->rectTransform->SetAnchorMin(Vector2(0, 0));
    logo->rectTransform->SetAnchorMax(Vector2(0, 0));
    logo->rectTransform->SetAnchoredPosition(Vector2(100, 100));
    logo->rectTransform->SetSizeDelta(Vector2(256, 256));
    logo->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    uiCanvas->AddUIElement(logo);

    GameObject* buttonGO = scene.CreateGameObject();
    Image* button = buttonGO->AddComponent<Image>();
    button->SetTexture(uiTexture);
    button->SetLayer(2);
    button->rectTransform->SetAnchorMin(Vector2(0, 0));
    button->rectTransform->SetAnchorMax(Vector2(0, 0));
    button->rectTransform->SetAnchoredPosition(Vector2(800, 500));
    button->rectTransform->SetSizeDelta(Vector2(200, 80));
    button->SetColor(glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
    uiCanvas->AddUIElement(button);

    GameObject* titleGO = scene.CreateGameObject();
    Text* title = titleGO->AddComponent<Text>(*arialFont, "Welcome to UI System");
    title->SetLayer(3);
    title->rectTransform->SetAnchorMin(Vector2(0, 0));
    title->rectTransform->SetAnchorMax(Vector2(0, 0));
    title->rectTransform->SetAnchoredPosition(Vector2(400, 50));
    title->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    uiCanvas->AddUIElement(title);

    title->rectTransform->SetSizeDelta(Vector2(800, 100));

    uiRenderer->RegisterRenderComponent(uiCanvas);
}

void SaveScene(const Scene& scene) {
    SceneSerializer serializer;
    SceneDeserializer deserializer;
    SceneCreator sceneCreator(serializer, deserializer);
    
    std::string scenesPath = "Assets/scenes";
    
    std::string sceneFilePath = scenesPath + "/test_scene1.scene";
    bool saved = sceneCreator.UpdateSceneFile(sceneFilePath, scene);
    
    if (saved) {
        std::cout << "Scene saved successfully to: " << sceneFilePath << std::endl;
    } 
    else {
        std::cout << "Failed to save scene!" << std::endl;
    }
}

void RegisterAllComponents(){
    ComponentRegistry::Register<Transform3D>("Transform3D");
    ComponentRegistry::Register<ModelComponent>("ModelComponent");
    ComponentRegistry::Register<PhysicalBody>("PhysicalBody");
    ComponentRegistry::Register<Polyhedron3D>("Polyhedron3D");
    ComponentRegistry::Register<Image>("Image");
    ComponentRegistry::Register<Text>("Text");
    ComponentRegistry::Register<Canvas>("Canvas"); 
    ComponentRegistry::Register<RectTransform>("RectTransform"); 
    ComponentRegistry::Register<TestComponent>("TestComponent");
}

void SetNameForObjects(const Scene& scene){
    std::vector<GameObject*> objects = scene.GetGameObjects();

    std::unordered_map<std::string, int> nameCount;
    std::unordered_map<GameObject*, std::string> objectToName;
    
    for (auto* object : objects) {
        std::string name = object->GetName();
        objectToName[object] = name;
        nameCount[name]++;
    }
    
    std::unordered_map<std::string, int> nameIndex;
    
    for (auto* object : objects) {
        std::string originalName = objectToName[object];
        
        if (nameCount[originalName] > 1) {
            std::string newName = originalName + "_" + std::to_string(nameIndex[originalName]);
            object->SetName(newName);
            nameIndex[originalName]++;
        }
    }
}

int main() {
    srand(time(nullptr));

    Time::Initialize();

    RegisterAllComponents();
    
    float clearColor[4] = {0.1f, 0.1f, 0.15f, 1.0f};
    Window* window = new Window(1600, 900, clearColor, "BEAR ENGINE - 3D Platformer");
    if (!window->Initialize()) return -1;

    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 10, 20));
    graphics.SetCamera(camera);

    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    UIRendering* uiRenderer = graphics.AddRender<UIRendering>();
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();

    PhysicsWorld physicsWorld;
    
    Scene mainScene("MainScene");
    
    ResourceManager* resources = mainScene.GetResourceManager();
    
    resources->LoadResource("Assets/models/cube.obj", ResourceType::Model);
    resources->LoadResource("Assets/models/platform.obj", ResourceType::Model);
    resources->LoadResource("Assets/ui/icon.png", ResourceType::Texture);
    resources->LoadResource("Assets/ui/arial.ttf", ResourceType::Font);

    Model* cubeModel = resources->GetResourceAs<Model>("Assets/models/cube.obj");
    
    if (!cubeModel || !cubeModel->IsLoaded()) {
        std::cout << "Failed to load model!" << std::endl;
        delete window;
        return -1;
    }
    
    float green[3] = {0.2, 1, 0.2};
    float blue[3] = {0.2, 0.2, 1};
    float gold[3] = {1, 0.8, 0};
    
    DirectionalLight3D* dirLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -2, -1).normalized(), 
        Vector3(1, 1, 1), 0.8f
    );
    dirLight->SetShadowArea(50.0f);
    dirLight->SetShadowPlanes(1.0f, 100.0f);
    
    std::vector<PointLight3D*> dynamicLights;
    for (int i = 0; i < 3; i++) {
        PointLight3D* light = renderer->AddLight<PointLight3D>(
            Vector3(i * 5 - 5, 3, i * 3 - 3),
            Vector3(0.2f + i * 0.2f, 0.3f, 0.8f),
            1.2f,
            15.0f
        );
        dynamicLights.push_back(light);
    }
    
    GameObject* player = mainScene.CreateGameObject();
    player->SetName("Player");

    Transform3D* playerTransform = player->AddComponent<Transform3D>();
    playerTransform->SetLocalPosition(Vector3(0, 2, 0));
    playerTransform->SetLocalScale(Vector3(0.8f, 1.2f, 0.8f));

    TestComponent* testComp = player->AddComponent<TestComponent>();
    testComp->SetTransform(playerTransform);
    
    Polyhedron3D* playerCollider = CreateCubeCollider(1.0f);
    playerCollider->SetTag("Player");
    player->AddComponent(playerCollider);
    
    PhysicalBody* playerBody = player->AddComponent<PhysicalBody>(playerCollider, 2.0f);
    playerBody->SetPhysicWorld(&physicsWorld);
    
    auto* playerImpulse = playerBody->AddFeature<ImpulseModule>();
    playerImpulse->SetMode(ImpulseModuleMode::ARCADY);
    playerImpulse->SetMaxSpeed(8.0f);
    playerImpulse->SetDamping(0.9f);
    
    playerBody->AddFeature<Gravity>();
    playerBody->AddFeature<Friction>(0.3f);
    
    ModelComponent* playerModel = new ModelComponent(*resources, *renderer, 
                                                     "Assets/models/cube.obj", green);
    player->AddComponent(playerModel);
    renderer->RegisterRenderComponent(playerModel);
    
    struct Platform {
        GameObject* obj;
        Transform3D* trans;
        BaseCollider* collider;
        PhysicalBody* body;
    };
    std::vector<Platform> platforms;
    
    GameObject* ground = mainScene.CreateGameObject();
    Transform3D* groundTrans = ground->AddComponent<Transform3D>();
    groundTrans->SetLocalPosition(Vector3(0, -1, 0));
    groundTrans->SetLocalScale(Vector3(30, 1, 30));
    
    Polyhedron3D* groundCollider = CreateCubeCollider(1.0f);
    groundCollider->SetTag("Ground");
    ground->AddComponent(groundCollider);
    
    PhysicalBody* groundBody = ground->AddComponent<PhysicalBody>(groundCollider, 0.0f);
    groundBody->SetPhysicWorld(&physicsWorld);
    
    ModelComponent* groundModel = new ModelComponent(*resources, *renderer, 
                                                     "Assets/models/slon_sea.obj", blue);
    ground->AddComponent(groundModel);
    renderer->RegisterRenderComponent(groundModel);
    
    physicsWorld.AddBody(groundBody);
    platforms.push_back({ground, groundTrans, groundCollider, groundBody});
    
    std::vector<Vector3> platformPositions = {
        Vector3(-10, 2, -5), Vector3(-5, 4, -8), Vector3(0, 6, -5),
        Vector3(5, 8, -2), Vector3(10, 10, 0), Vector3(-8, 12, 5),
        Vector3(-3, 14, 8), Vector3(3, 16, 10), Vector3(9, 18, 7)
    };
    
    for (size_t i = 0; i < platformPositions.size(); i++) {
        GameObject* plat = mainScene.CreateGameObject();
        Transform3D* platTrans = plat->AddComponent<Transform3D>();
        platTrans->SetLocalPosition(platformPositions[i]);
        platTrans->SetLocalScale(Vector3(2, 0.5f, 2));
        
        Polyhedron3D* platCollider = CreateCubeCollider(1.0f);
        platCollider->SetTag("Platform");
        plat->AddComponent(platCollider);
        
        PhysicalBody* platBody = plat->AddComponent<PhysicalBody>(platCollider, 0.0f);
        platBody->SetPhysicWorld(&physicsWorld);
        
        float color[3] = {0.3f + i * 0.05f, 0.3f, 0.5f};
        ModelComponent* platModel = new ModelComponent(*resources, *renderer, 
                                                       "Assets/models/slon_sea.obj", color);
        plat->AddComponent(platModel);
        renderer->RegisterRenderComponent(platModel);
        
        physicsWorld.AddBody(platBody);
        platforms.push_back({plat, platTrans, platCollider, platBody});
    }
    
    std::vector<GameObject*> collectibles;
    std::vector<PhysicalBody*> collectibleBodies;
    std::vector<Vector3> coinPositions = {
        Vector3(-8, 3, -3), Vector3(-3, 5, -6), Vector3(2, 7, -3),
        Vector3(7, 9, 0), Vector3(12, 11, 2), Vector3(-6, 13, 7),
        Vector3(0, 15, 9), Vector3(5, 17, 8), Vector3(11, 19, 6)
    };
    
    for (const auto& pos : coinPositions) {
        GameObject* coin = mainScene.CreateGameObject();
        Transform3D* coinTrans = coin->AddComponent<Transform3D>();
        coinTrans->SetLocalPosition(pos);
        coinTrans->SetLocalScale(Vector3(0.5f, 0.5f, 0.5f));
        
        Polyhedron3D* coinCollider = CreateCubeCollider(0.5f);
        coinCollider->SetTag("Collectible");
        coin->AddComponent(coinCollider);
        
        PhysicalBody* coinBody = coin->AddComponent<PhysicalBody>(coinCollider, 0.0f);
        coinBody->SetPhysicWorld(&physicsWorld);
        
        ModelComponent* coinModel = new ModelComponent(*resources, *renderer, 
                                                       "Assets/models/cube.obj", gold);
        coin->AddComponent(coinModel);
        renderer->RegisterRenderComponent(coinModel);
        
        physicsWorld.AddBody(coinBody);
        collectibles.push_back(coin);
        collectibleBodies.push_back(coinBody);
    }
    
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    colliderManager->AddCollider(playerCollider);
    for (auto& p : platforms) {
        colliderManager->AddCollider(p.collider);
    }
    for (auto* coin : collectibles) {
        colliderManager->AddCollider(coin->GetComponentOfType<BaseCollider>());
    }
    
    physicsWorld.Start();
    
    int score = 0;
    int totalCoins = collectibles.size();
    
    playerCollider->SubscribeToCollision(CollisionEvent::State::ENTER,
        [&score, totalCoins, &collectibles, &collectibleBodies, &physicsWorld](BaseCollider* self, BaseCollider* other) {
            if (other->GetTag() == "Collectible") {
                score++;
                std::cout << "Монеток: " << score << "/" << totalCoins << "\r";
                std::cout.flush();
                
                for (size_t i = 0; i < collectibles.size(); i++) {
                    if (collectibles[i]->GetComponentOfType<BaseCollider>() == other) {
                        physicsWorld.RemoveBody(collectibleBodies[i]);
                        collectibles[i]->Destroy();
                        collectibles.erase(collectibles.begin() + i);
                        collectibleBodies.erase(collectibleBodies.begin() + i);
                        break;
                    }
                }
            }
        });
    
    VisualMode currentMode = VisualMode::NORMAL;
    bool showColliders = false;
    
    std::cout << "\n=== 3D PLATFORMER - РЕЖИМЫ ВИЗУАЛИЗАЦИИ ===\n";
    std::cout << "F1 - Нормальный режим\n";
    std::cout << "F2 - Режим треугольников (wireframe)\n";
    std::cout << "F3 - Режим коллайдеров\n";
    std::cout << "F4 - Режим источников света\n";
    std::cout << "==========================================\n";
    std::cout << "Управление: WASD - движение, Space - прыжок\n";
    std::cout << "Стрелки - камера, Собрано: 0/" << totalCoins << "\n";
    
    float time = 0;

    TransparencyModelRenderer* transRenderer = new TransparencyModelRenderer();
    renderer->RegisterModelFeatureRenderer(transRenderer);

    GameObject* transparentCube = mainScene.CreateGameObject();
    Transform3D* transTransform = transparentCube->AddComponent<Transform3D>();
    transTransform->SetLocalPosition(Vector3(3, 3, 2));
    transTransform->SetLocalScale(Vector3(1.5f, 1.5f, 1.5f));

    float transparentColor[3] = {1.0f, 0.2f, 0.2f};

    ModelComponent* transModel = new ModelComponent(*resources, *renderer, 
        "Assets/models/cube.obj", transparentColor);

    TransparentFeature* transFeature = new TransparentFeature();
    transFeature->SetAlpha(0.5f); 
    transModel->AddFeature(transFeature);

    transparentCube->AddComponent(transModel);
    renderer->RegisterRenderComponent(transModel);

    GameObject* transparentCube2 = mainScene.CreateGameObject();
    Transform3D* transTransform2 = transparentCube2->AddComponent<Transform3D>();
    transTransform2->SetLocalPosition(Vector3(-3, 2, 4));
    transTransform2->SetLocalScale(Vector3(1.2f, 1.2f, 1.2f));

    float transparentColor2[3] = {0.2f, 0.2f, 1.0f};
    ModelComponent* transModel2 = new ModelComponent(*resources, *renderer, 
        "Assets/models/cube.obj", transparentColor2);

    TransparentFeature* transFeature2 = new TransparentFeature();
    transFeature2->SetAlpha(0.3f);
    transModel2->AddFeature(transFeature2);

    transparentCube2->AddComponent(transModel2);

    renderer->RegisterModelFeatureRenderer(new SimpleModelRenderer());
    renderer->RegisterRenderComponent(transModel2);
    
    InitUI(uiRenderer, *resources, mainScene);
    
    mainScene.InitializeScene();
    mainScene.StartScene();
    
    SetNameForObjects(mainScene);
    SaveScene(mainScene);

    while (!window->ShouldClose() && !input.GetKeyDown(Keys::Escape)) {
        Time::Tick();
        float deltaTime = Time::DeltaTime();
        time += deltaTime;
        
        input.Update();
        
        float moveSpeed = 5.0f * deltaTime;
        
        Vector3 newPos = playerTransform->GetLocalPosition();
        
        if (input.GetKey(Keys::W)) newPos.z -= moveSpeed;
        if (input.GetKey(Keys::S)) newPos.z += moveSpeed;
        if (input.GetKey(Keys::A)) newPos.x -= moveSpeed;
        if (input.GetKey(Keys::D)) newPos.x += moveSpeed;
        
        static float verticalVelocity = 0;
        static bool isGrounded = false;
        
        isGrounded = newPos.y <= 2.1f;
        
        if (input.GetKeyDown(Keys::Space) && isGrounded) {
            verticalVelocity = 5.0f;
            std::cout << "ПРЫЖОК!\n";
        }
        
        verticalVelocity -= 9.8f * deltaTime;
        newPos.y += verticalVelocity * deltaTime;
        
        if (newPos.y < 2.0f) {
            newPos.y = 2.0f;
            verticalVelocity = 0;
        }
        
        if (newPos.x > 15) newPos.x = 15;
        if (newPos.x < -15) newPos.x = -15;
        if (newPos.z > 15) newPos.z = 15;
        if (newPos.z < -15) newPos.z = -15;
        
        playerTransform->SetLocalPosition(newPos);
        
        float cameraSpeed = 8.0f * deltaTime;
        Vector3 camPos = camera->position;
        if (input.GetKey(Keys::Up)) camPos.z -= cameraSpeed;
        if (input.GetKey(Keys::Down)) camPos.z += cameraSpeed;
        if (input.GetKey(Keys::Left)) camPos.x -= cameraSpeed;
        if (input.GetKey(Keys::Right)) camPos.x += cameraSpeed;
        camera->position = camPos;
        
        Vector3 targetPos = playerTransform->GetLocalPosition() + Vector3(0, 5, 10);
        camera->position = camera->position * 0.95f + targetPos * 0.05f;
        
        if (input.GetKeyDown(Keys::F1)) {
            currentMode = VisualMode::NORMAL;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            showColliders = false;
            std::cout << "Режим: NORMAL\n";
        }
        if (input.GetKeyDown(Keys::F2)) {
            currentMode = VisualMode::WIREFRAME;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            showColliders = false;
            std::cout << "Режим: WIREFRAME\n";
        }
        if (input.GetKeyDown(Keys::F3)) {
            currentMode = VisualMode::COLLIDERS;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            showColliders = true;
            std::cout << "Режим: COLLIDERS\n";
        }
        if (input.GetKeyDown(Keys::F4)) {
            currentMode = VisualMode::LIGHTS;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            showColliders = false;
            std::cout << "Режим: LIGHTS\n";
        }
        
        for (size_t i = 0; i < dynamicLights.size(); i++) {
            float offset = i * 2.0f;
            Vector3 pos(
                sin(time * 0.5f + offset) * 8.0f,
                3.0f + sin(time * 0.8f + offset) * 2.0f,
                cos(time * 0.5f + offset) * 8.0f
            );
            dynamicLights[i]->SetLocalPosition(pos);
        }
        
        static bool firstMouse = true;
        static float lastX, lastY;
        
        if (input.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            double xpos = input.GetMouseX();
            double ypos = input.GetMouseY();
            
            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }
            
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            
            lastX = xpos;
            lastY = ypos;
            
            camera->ProcessMouse(xoffset, yoffset);
        } else {
            firstMouse = true;
        }
        
        physicsWorld.Update();
        mainScene.UpdateScene();
        
        window->Clear();
        graphics.Update();
        uiRenderer->Update();
        window->SwapBuffers();
    }
    
    mainScene.DestroyScene();
    
    return 0;
}