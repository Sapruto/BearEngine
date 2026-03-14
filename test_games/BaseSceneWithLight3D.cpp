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
#include "Polygon3D.h"
#include "Newtonian.h"
#include "Gravity.h"
#include "CollisionReaction.h"
#include "Friction.h"
#include "ElasticDeformation.h"

#include "DirectionalLight3D.h"
#include "PointLight3D.h"

#include <cstdlib> 
#include <ctime> 

int main() {
    srand(time(nullptr));

    Time::Initialize();
    
    float clearColor[4] = {0.05f, 0.05f, 0.1f, 1.0f};
    Window* window = new Window(1280, 720, clearColor, "BearEngine Physics Demo");
    if (!window->Initialize()) return -1;

    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 8, 25));
    graphics.SetCamera(camera);

    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    ResourceManager resources;
    
    resources.LoadResource("Assets/models/cube.obj", ResourceType::Model);
    resources.LoadResource("Assets/models/slon_sea.obj", ResourceType::Model);
    Model* cubeModel = resources.GetResourceAs<Model>("Assets/models/cube.obj");
    
    if (!cubeModel || !cubeModel->IsLoaded()) {
        std::cout << "Failed to load model!" << std::endl;
        return -1;
    }
    
    PhysicsWorld physicsWorld;
    
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    
    
    float red[3] = {1, 0.2, 0.2};
    float green[3] = {0.2, 1, 0.2};
    float blue[3] = {0.2, 0.2, 1};
    float yellow[3] = {1, 1, 0.2};
    float white[3] = {1, 1, 1};
    
    
    DirectionalLight3D* dirLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -1, -1).normalized(), 
        Vector3(1, 1, 1),    
        0.6f                 
    );
    dirLight->SetShadowArea(30.0f);
    dirLight->SetShadowPlanes(1.0f, 70.0f);
    
    PointLight3D* pointLight1 = renderer->AddLight<PointLight3D>(
        Vector3(5, 5, 5),    
        Vector3(1, 0.3, 0.3),   
        1.2f,               
        15.0f              
    );
    
    PointLight3D* pointLight2 = renderer->AddLight<PointLight3D>(
        Vector3(-5, 3, -5),
        Vector3(0.3, 0.5, 1),    
        1.2f,             
        15.0f                
    );
    
    
    GameObject* platform = new GameObject();
    Transform3D* tPlatform = platform->AddComponent<Transform3D>();
    tPlatform->position = Vector3(0, -2, 0);
    tPlatform->scale = Vector3(20.0f, 0.5f, 20.0f);
    
    Polygon3D* platformCollider = new Polygon3D();
    *platformCollider = Polygon3D::CreateRectangle(20.0f, 0.5f, 20.0f);
    platform->AddComponent(platformCollider);
    
    PhysicalBody* platformBody = platform->AddComponent<PhysicalBody>(platformCollider, 1000.0f);
    
    
    GameObject* cube1 = new GameObject();
    Transform3D* tCube1 = cube1->AddComponent<Transform3D>();
    tCube1->position = Vector3(-3, 5, 2);
    tCube1->scale = Vector3(0.6f, 0.6f, 0.6f);
    
    Polygon3D* cube1Collider = new Polygon3D();
    *cube1Collider = Polygon3D::CreateRectangle(0.6f, 0.6f, 0.6f);
    cube1->AddComponent(cube1Collider);
    
    PhysicalBody* cube1Body = cube1->AddComponent<PhysicalBody>(cube1Collider, 1.5f);
    
    auto* impulse1 = cube1Body->AddFeature<ImpulseModule>();
    impulse1->SetMode(ImpulseModuleMode::ARCADY);
    impulse1->SetMaxSpeed(12.0f);
    impulse1->SetDamping(0.99f);
    
    //cube1Body->AddFeature<Gravity>();
    cube1Body->AddFeature<CollisionReaction>();
    cube1Body->AddFeature<Friction>(0.7f);  
    
    
    GameObject* cube2 = new GameObject();
    Transform3D* tCube2 = cube2->AddComponent<Transform3D>();
    tCube2->position = Vector3(0, 8, -2);
    tCube2->scale = Vector3(0.7f, 0.7f, 0.7f);
    
    Polygon3D* cube2Collider = new Polygon3D();
    *cube2Collider = Polygon3D::CreateRectangle(0.7f, 0.7f, 0.7f);
    cube2->AddComponent(cube2Collider);
    
    PhysicalBody* cube2Body = cube2->AddComponent<PhysicalBody>(cube2Collider, 1.2f);
    
    auto* impulse2 = cube2Body->AddFeature<ImpulseModule>();
    impulse2->SetMode(ImpulseModuleMode::REALISTIC);
    impulse2->SetMaxSpeed(10.0f);
    impulse2->SetDamping(0.98f);
    
    cube2Body->AddFeature<Gravity>();
    
    
    std::vector<Vector3> bindingPoints = {
        Vector3(0.35, 0.35, 0.35),
        Vector3(-0.35, 0.35, 0.35),
        Vector3(0.35, -0.35, 0.35),
        Vector3(-0.35, -0.35, 0.35),
        Vector3(0.35, 0.35, -0.35),
        Vector3(-0.35, 0.35, -0.35),
        Vector3(0.35, -0.35, -0.35),
        Vector3(-0.35, -0.35, -0.35)
    };
    
    cube2Body->AddFeature<ElasticDeformation>(50.0f, bindingPoints);
    
    
    GameObject* cube3 = new GameObject();
    Transform3D* tCube3 = cube3->AddComponent<Transform3D>();
    tCube3->position = Vector3(4, 3, -3);
    tCube3->scale = Vector3(0.5f, 0.5f, 0.5f);
    
    Polygon3D* cube3Collider = new Polygon3D();
    *cube3Collider = Polygon3D::CreateRectangle(0.5f, 0.5f, 0.5f);
    cube3->AddComponent(cube3Collider);
    
    PhysicalBody* cube3Body = cube3->AddComponent<PhysicalBody>(cube3Collider, 0.8f);
    
    auto* impulse3 = cube3Body->AddFeature<ImpulseModule>();
    impulse3->SetMode(ImpulseModuleMode::REALISTIC);
    impulse3->SetMaxSpeed(15.0f);
    impulse3->SetDamping(0.99f);
    
    cube3Body->AddFeature<Gravity>();
    cube3Body->AddFeature<CollisionReaction>();
    
    
    ModelComponent* mPlatform = new ModelComponent(resources, *renderer, "Assets/models/platform.obj", green);
    platform->AddComponent(mPlatform);
    
    ModelComponent* mCube1 = new ModelComponent(resources, *renderer, "Assets/models/slon_sea.obj", red);
    cube1->AddComponent(mCube1);
    
    ModelComponent* mCube2 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", blue);
    cube2->AddComponent(mCube2);
    
    ModelComponent* mCube3 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", yellow);
    cube3->AddComponent(mCube3);
    
    
    renderer->RegisterRenderComponent(mPlatform);
    renderer->RegisterRenderComponent(mCube1);
    renderer->RegisterRenderComponent(mCube2);
    renderer->RegisterRenderComponent(mCube3);
    
    
    physicsWorld.AddBody(platformBody);
    physicsWorld.AddBody(cube1Body);
    physicsWorld.AddBody(cube2Body);
    physicsWorld.AddBody(cube3Body);
    physicsWorld.Start();
    
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    colliderManager->AddCollider(platformCollider);
    colliderManager->AddCollider(cube1Collider);
    colliderManager->AddCollider(cube2Collider);
    colliderManager->AddCollider(cube3Collider);
    
    float time = 0.0f;
    bool wireframeMode = false;
    
    std::cout << "=== BEARENGINE CUBE PHYSICS DEMO ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD + Space/Shift - Move camera" << std::endl;
    std::cout << "  Right Mouse Button + Drag - Look around" << std::endl;
    std::cout << "  F - Apply force to RED cube (friction)" << std::endl;
    std::cout << "  G - Apply force to BLUE cube (elastic)" << std::endl;
    std::cout << "  H - Apply force to YELLOW cube (normal)" << std::endl;
    std::cout << "  R - Reset all cubes" << std::endl;
    std::cout << "  T - Toggle wireframe" << std::endl;
    std::cout << "  L - Toggle directional light" << std::endl;
    std::cout << "  P - Print positions" << std::endl;
    std::cout << "====================================" << std::endl;
    
    while (!window->ShouldClose()) {
        Time::Tick();
        time += Time::DeltaTime();
        
        input.Update();
        
        
        float cameraSpeed = 8.0f * Time::DeltaTime();
        if (input.GetKey(Keys::W)) camera->position += camera->front * cameraSpeed;
        if (input.GetKey(Keys::S)) camera->position -= camera->front * cameraSpeed;
        if (input.GetKey(Keys::A)) camera->position -= camera->right * cameraSpeed;
        if (input.GetKey(Keys::D)) camera->position += camera->right * cameraSpeed;
        if (input.GetKey(Keys::Space)) camera->position.y += cameraSpeed;
        if (input.GetKey(Keys::LeftShift)) camera->position.y -= cameraSpeed;
        
        
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
        
        
        if (pointLight1) {
            pointLight1->SetPosition(Vector3(
                6.0f * cos(time * 0.8f),
                4.0f + sin(time * 1.5f) * 2.0f,
                6.0f * sin(time * 0.8f)
            ));
        }
        
        if (pointLight2) {
            pointLight2->SetPosition(Vector3(
                -5.0f + cos(time * 1.2f) * 3.0f,
                3.0f + sin(time * 1.2f) * 1.5f,
                -5.0f + sin(time * 1.2f) * 3.0f
            ));
        }

        if (input.GetKeyDown(Keys::Q)) {
            GameObject* cube = new GameObject();
            Transform3D* tCube = cube->AddComponent<Transform3D>();
            tCube->position = Vector3(
                (rand() % 11) * 1.0f,        
                (rand() % 11) * 1.0f,      
                (rand() % 11) * 1.0f    
            );
            tCube->scale = Vector3(1.0f, 1.0f, 1.0f);
            
            Polygon3D* cubeCollider = new Polygon3D();
            *cubeCollider = Polygon3D::CreateRectangle(0.5f, 0.5f, 0.5f);
            cube->AddComponent(cubeCollider);
            
            PhysicalBody* cubeBody = cube->AddComponent<PhysicalBody>(cubeCollider, 0.8f);
            
            auto* impulse3 = cubeBody->AddFeature<ImpulseModule>();
            impulse3->SetMode(ImpulseModuleMode::REALISTIC);
            impulse3->SetMaxSpeed(15.0f);
            impulse3->SetDamping(0.99f);
            
            cubeBody->AddFeature<CollisionReaction>();
            
            ModelComponent* mCube = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", yellow);
            cube->AddComponent(mCube);
            
            renderer->RegisterRenderComponent(mCube);
            
            physicsWorld.AddBody(cubeBody);
        }
        
        
        if (input.GetKeyDown(Keys::F)) {
            if (auto* imp = cube1Body->GetFeatureOfType<ImpulseModule>()) {
                imp->AddForce(Vector3(1, 0.5f, 0), 15.0f);
                std::cout << "Force to RED cube (friction)! Pos: " 
                          << tCube1->position << std::endl;
            }
        }

        if (input.GetKeyDown(Keys::X)) {
            if (auto* imp = cube1Body->GetFeatureOfType<ImpulseModule>()) {
                Force newForce = Force(Vector3::Zero, 0.0f);
                imp->ReForce(newForce);
            }
        }
        
        if (input.GetKeyDown(Keys::G)) {
            if (auto* imp = cube2Body->GetFeatureOfType<ImpulseModule>()) {
                imp->AddForce(Vector3(-1, 0.3f, 1), 12.0f);
                std::cout << "Force to BLUE cube (elastic)! Pos: " 
                          << tCube2->position << std::endl;
            }
        }
        
        if (input.GetKeyDown(Keys::H)) {
            if (auto* imp = cube3Body->GetFeatureOfType<ImpulseModule>()) {
                imp->AddForce(Vector3(0, 1, 1), 10.0f);
                std::cout << "Force to YELLOW cube! Pos: " 
                          << tCube3->position << std::endl;
            }
        }
        
        
        if (input.GetKeyDown(Keys::R)) {
            tCube1->position = Vector3(-3, 5, 2);
            tCube2->position = Vector3(0, 8, -2);
            tCube3->position = Vector3(4, 3, -3);
            
            if (auto* imp = cube1Body->GetFeatureOfType<ImpulseModule>()) {
                imp->SetVelocity(Vector3::Zero);
            }
            if (auto* imp = cube2Body->GetFeatureOfType<ImpulseModule>()) {
                imp->SetVelocity(Vector3::Zero);
            }
            if (auto* imp = cube3Body->GetFeatureOfType<ImpulseModule>()) {
                imp->SetVelocity(Vector3::Zero);
            }
            
            std::cout << "All cubes reset!" << std::endl;
        }
        
        
        if (input.GetKey(Keys::Up)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(0, 0.02f, 0));
        }
        if (input.GetKey(Keys::Down)) {
            dirLight->SetDirection(dirLight->GetDirection() - Vector3(0, 0.02f, 0));
        }
        if (input.GetKey(Keys::Left)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(-0.02f, 0, 0));
        }
        if (input.GetKey(Keys::Right)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(0.02f, 0, 0));
        }
        
        static bool dirLightEnabled = true;
        if (input.GetKeyDown(Keys::L)) {
            dirLightEnabled = !dirLightEnabled;
            dirLight->SetIntensity(dirLightEnabled ? 0.6f : 0.0f);
            std::cout << "Light: " << (dirLightEnabled ? "ON" : "OFF") << std::endl;
        }
        
        
        if (input.GetKeyDown(Keys::T)) {
            wireframeMode = !wireframeMode;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
            std::cout << "Wireframe: " << (wireframeMode ? "ON" : "OFF") << std::endl;
        }
        
        
        if (input.GetKeyDown(Keys::P)) {
            std::cout << "\n=== CUBE POSITIONS ===" << std::endl;
            std::cout << "Red (friction): " << tCube1->position << std::endl;
            std::cout << "Blue (elastic): " << tCube2->position << std::endl;
            std::cout << "Yellow (normal): " << tCube3->position << std::endl;
            std::cout << "=====================\n" << std::endl;
        }
        
        physicsWorld.Update();
        colliderManager->ProcessEvents();
        
        window->Clear();
        graphics.Update();
        window->SwapBuffers();
    }
    
    return 0;
}