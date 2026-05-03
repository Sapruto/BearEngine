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
#include <fstream>

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

    Transform3D* GetTransform() { 
        return trans.GetValue();
    }

    SERIALIZED_FIELDS(&trans, &pos)
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

std::string ReadSceneFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    srand(time(nullptr));
    Time::Initialize();
    RegisterAllComponents();
    
    float clearColor[4] = {0.05f, 0.05f, 0.1f, 1.0f};
    Window* window = new Window(1600, 900, clearColor, "BEAR ENGINE - 3D Platformer");
    if (!window->Initialize()) return -1;

    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 12, 20));
    graphics.SetCamera(camera);

    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    UIRendering* uiRenderer = graphics.AddRender<UIRendering>();
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    PhysicsWorld physicsWorld;
    
    SceneDeserializer deserializer;
    std::string sceneText = ReadSceneFile("Assets/scenes/test_scene1.scene");
    if (sceneText.empty()) return -1;
    
    std::unique_ptr<Scene> mainScene = deserializer.GenerateSceneThroughFile(sceneText);
    if (!mainScene) return -1;
    
    ResourceManager* resources = mainScene->GetResourceManager();
    resources->LoadResource("Assets/models/cube.obj", ResourceType::Model);
    resources->LoadResource("Assets/models/slon_sea.obj", ResourceType::Model);
    resources->LoadResource("Assets/ui/icon.png", ResourceType::Texture);
    resources->LoadResource("Assets/ui/arial.ttf", ResourceType::Font);
    
    Transform3D* playerTransform = nullptr;
    ModelComponent* playerModel = nullptr;
    PhysicalBody* playerBody = nullptr;
    
    for (auto* obj : mainScene->GetGameObjects()) {
        if (obj->GetName() == "Player") {
            playerTransform = obj->GetComponentOfType<Transform3D>();
            playerModel = obj->GetComponentOfType<ModelComponent>();
            playerBody = obj->GetComponentOfType<PhysicalBody>();
            
            if (playerModel) {
                float green[3] = {0.2f, 0.9f, 0.2f};
                playerModel->SetColor(green);
                renderer->RegisterRenderComponent(playerModel);
            }
            
            if (playerBody) {
                playerBody->SetPhysicWorld(&physicsWorld);
                playerBody->AddFeature<ImpulseModule>();
                playerBody->AddFeature<Gravity>();
                playerBody->AddFeature<Friction>(0.3f);
                physicsWorld.AddBody(playerBody);
            }
        }
        
        ModelComponent* model = obj->GetComponentOfType<ModelComponent>();
        if (model && obj->GetName() != "Player") {
            if (obj->GetName() == "_0") {
                float gray[3] = {0.4f, 0.4f, 0.4f};
                model->SetColor(gray);
            } else if (obj->GetName().find("_") != std::string::npos && obj->GetName() != "_0") {
                float brown[3] = {0.6f, 0.4f, 0.2f};
                model->SetColor(brown);
            }
            renderer->RegisterRenderComponent(model);
        }
        
        PhysicalBody* body = obj->GetComponentOfType<PhysicalBody>();
        if (body && obj->GetName() != "Player") {
            body->SetPhysicWorld(&physicsWorld);
            physicsWorld.AddBody(body);
        }
        
        Polyhedron3D* collider = obj->GetComponentOfType<Polyhedron3D>();
        if (collider && obj->GetName() == "Player") {
            collider->SetTag("Player");
        }
    }
    
    DirectionalLight3D* sunLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -2, -1).normalized(), 
        Vector3(1, 1, 1), 1.0f
    );
    sunLight->SetShadowArea(40.0f);
    sunLight->SetShadowPlanes(1.0f, 50.0f);
    
    PointLight3D* ambientLight = renderer->AddLight<PointLight3D>(
        Vector3(0, 10, 0), Vector3(0.3f, 0.3f, 0.4f), 0.5f, 50.0f
    );
    
    PointLight3D* playerLight = renderer->AddLight<PointLight3D>(
        Vector3(0, 2, 0), Vector3(1.0f, 0.5f, 0.2f), 1.2f, 12.0f
    );
    
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    for (auto* obj : mainScene->GetGameObjects()) {
        Polyhedron3D* collider = obj->GetComponentOfType<Polyhedron3D>();
        if (collider) colliderManager->AddCollider(collider);
    }
    
    physicsWorld.Start();
    mainScene->LoadResources({});
    mainScene->InitializeScene();
    mainScene->StartScene();
    
    std::cout << "\n==================================================\n";
    std::cout << "   3D PLATFORMER - ЗАГРУЖЕНА СОХРАНЕННАЯ СЦЕНА\n";
    std::cout << "==================================================\n";
    std::cout << "WASD - движение | SPACE - прыжок\n";
    std::cout << "Стрелки - движение камеры | ПКМ + мышь - поворот\n";
    std::cout << "F1 - заливка | F2 - каркас\n";
    std::cout << "==================================================\n\n";
    
    float time = 0;
    float groundY = 2.0f;
    float verticalVelocity = 0;
    bool isGrounded = false;
    
    for (auto* obj : mainScene->GetGameObjects()) {
        if (obj->GetName() == "_0") {
            Transform3D* groundTrans = obj->GetComponentOfType<Transform3D>();
            if (groundTrans) groundY = groundTrans->GetLocalPosition().y + 1.2f;
            break;
        }
    }
    
    while (!window->ShouldClose() && !input.GetKeyDown(Keys::Escape)) {
        Time::Tick();
        float deltaTime = Time::DeltaTime();
        time += deltaTime;
        input.Update();
        
        if (playerTransform) {
            Vector3 newPos = playerTransform->GetLocalPosition();
            float moveSpeed = 7.0f * deltaTime;
            
            if (input.GetKey(Keys::W)) newPos.z -= moveSpeed;
            if (input.GetKey(Keys::S)) newPos.z += moveSpeed;
            if (input.GetKey(Keys::A)) newPos.x -= moveSpeed;
            if (input.GetKey(Keys::D)) newPos.x += moveSpeed;
            
            isGrounded = newPos.y <= groundY + 0.2f;
            
            if (input.GetKeyDown(Keys::Space) && isGrounded) {
                verticalVelocity = 6.5f;
            }
            
            verticalVelocity -= 10.0f * deltaTime;
            newPos.y += verticalVelocity * deltaTime;
            
            if (newPos.y < groundY) {
                newPos.y = groundY;
                verticalVelocity = 0;
            }
            
            float bound = 28.0f;
            newPos.x = std::max(-bound, std::min(bound, newPos.x));
            newPos.z = std::max(-bound, std::min(bound, newPos.z));
            
            playerTransform->SetLocalPosition(newPos);
            
            Vector3 targetPos = newPos + Vector3(0, 5, 14);
            camera->position = camera->position * 0.94f + targetPos * 0.06f;
            
            playerLight->SetLocalPosition(newPos + Vector3(0, 2, 0));
        }
        
        Vector3 camPos = camera->position;
        if (input.GetKey(Keys::Up)) camPos.z -= 10.0f * Time::DeltaTime();
        if (input.GetKey(Keys::Down)) camPos.z += 10.0f * Time::DeltaTime();
        if (input.GetKey(Keys::Left)) camPos.x -= 10.0f * Time::DeltaTime();
        if (input.GetKey(Keys::Right)) camPos.x += 10.0f * Time::DeltaTime();
        camera->position = camPos;
        
        if (input.GetKeyDown(Keys::F1)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (input.GetKeyDown(Keys::F2)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        static bool firstMouse = true;
        static float lastX = 0, lastY = 0;
        
        if (input.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            double xpos = input.GetMouseX();
            double ypos = input.GetMouseY();
            if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
            float xoffset = (xpos - lastX) * 0.15f;
            float yoffset = (lastY - ypos) * 0.15f;
            lastX = xpos; lastY = ypos;
            camera->ProcessMouse(xoffset, yoffset);
        } else {
            firstMouse = true;
        }
        
        float lightY = 5.0f + sin(time * 0.8f) * 2.0f;
        ambientLight->SetLocalPosition(Vector3(sin(time * 0.3f) * 8.0f, lightY, cos(time * 0.3f) * 8.0f));
        
        physicsWorld.Update();
        mainScene->UpdateScene();
        
        window->Clear();
        graphics.Update();
        uiRenderer->Update();
        window->SwapBuffers();
    }
    
    mainScene->DestroyScene();
    return 0;
}