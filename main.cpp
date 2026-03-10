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

#include "ColliderManager.h"
#include "PhysicsWorld.h"
#include "PhysicalBody.h"
#include "Polygon3D.h"
#include "Newtonian.h"
#include "Gravity.h"
#include "CollisionReaction.h"

#include "DirectionalLight3D.h"
#include "PointLight3D.h"

int main() {
    Time::Initialize();
    
    float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    Window* window = new Window(1280, 720, clearColor, "3D Test");
    if (!window->Initialize()) return -1;

    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    Camera3D* camera = new Camera3D(Vector3(0, 5, 30));
    graphics.SetCamera(camera);

    InputSystem& input = InputSystem::GetInstance();
    input.Initialize(window->GetWindow());
    
    ResourceManager resources;
    
    resources.LoadResource("Assets/models/cube.obj", ResourceType::Model);
    Model* cubeModel = resources.GetResourceAs<Model>("Assets/models/cube.obj");
    
    if (!cubeModel || !cubeModel->IsLoaded()) {
        std::cout << "Failed to load model!" << std::endl;
        return -1;
    }
    
    PhysicsWorld physicsWorld;
    
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    
    float red[3] = {1, 0, 0};
    float green[3] = {0, 1, 0};
    float blue[3] = {0, 0, 1};
    float white[3] = {1, 1, 1};
    
    DirectionalLight3D* dirLight = renderer->AddLight<DirectionalLight3D>(
        Vector3(-1, -1, -1), 
        Vector3(1, 1, 1),    
        0.8f                 
    );
    dirLight->SetShadowArea(20.0f);
    dirLight->SetShadowPlanes(1.0f, 50.0f);
    
    PointLight3D* pointLight1 = renderer->AddLight<PointLight3D>(
        Vector3(3, 3, 3),    
        Vector3(1, 0, 0),   
        1.0f,               
        10.0f              
    );
    
    PointLight3D* pointLight2 = renderer->AddLight<PointLight3D>(
        Vector3(-3, 2, -3),
        Vector3(0, 0, 1),    
        1.0f,             
        8.0f                
    );
    
    GameObject* platform = new GameObject();
    Transform3D* tPlatform = platform->AddComponent<Transform3D>();
    tPlatform->position = Vector3(0, -2, 0);
    tPlatform->scale = Vector3(10.0f, 0.5f, 10.0f);
    
    Polygon3D* platformCollider = new Polygon3D();
    *platformCollider = Polygon3D::CreateRectangle(10.0f, 0.5f, 10.0f);
    platform->AddComponent(platformCollider);
    
    PhysicalBody* platformBody = platform->AddComponent<PhysicalBody>(platformCollider, 1000.0f);
    
    GameObject* cube = new GameObject();
    Transform3D* tCube = cube->AddComponent<Transform3D>();
    tCube->position = Vector3(0, 5, 0);
    tCube->scale = Vector3(0.5f, 0.5f, 0.5f);
    
    Polygon3D* cubeCollider = new Polygon3D();
    *cubeCollider = Polygon3D::CreateRectangle(0.5f, 0.5f, 0.5f);
    cube->AddComponent(cubeCollider);
    
    PhysicalBody* cubeBody = cube->AddComponent<PhysicalBody>(cubeCollider, 1.0f);
    
    auto* impulse = cubeBody->AddFeature<ImpulseModule>();
    impulse->SetMode(ImpulseModuleMode::REALISTIC);
    impulse->SetMaxSpeed(15.0f);
    impulse->SetDamping(0.995f);
    impulse->SetWorldLimit(50.0f);
    
    cubeBody->AddFeature<Gravity>();
    cubeBody->AddFeature<CollisionReaction>();
    
    GameObject* cube2 = new GameObject();
    Transform3D* tCube2 = cube2->AddComponent<Transform3D>();
    tCube2->position = Vector3(2, 2, 2);
    tCube2->scale = Vector3(0.5f, 0.5f, 0.5f);
    
    Polygon3D* cube2Collider = new Polygon3D();
    *cube2Collider = Polygon3D::CreateRectangle(0.5f, 0.5f, 0.5f);
    cube2->AddComponent(cube2Collider);
    
    PhysicalBody* cube2Body = cube2->AddComponent<PhysicalBody>(cube2Collider, 1.0f);
    cube2Body->AddFeature<Gravity>();
    
    ModelComponent* mPlatform = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", green);
    platform->AddComponent(mPlatform);
    
    ModelComponent* mCube = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", white);
    cube->AddComponent(mCube);
    
    ModelComponent* mCube2 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", white);
    cube2->AddComponent(mCube2);
    
    renderer->RegisterRenderComponent(mPlatform);
    renderer->RegisterRenderComponent(mCube);
    renderer->RegisterRenderComponent(mCube2);
    
    physicsWorld.AddBody(platformBody);
    physicsWorld.AddBody(cubeBody);
    physicsWorld.AddBody(cube2Body);
    physicsWorld.Start();
    
    auto colliderManager = std::make_shared<ColliderManager>();
    physicsWorld.SetColliderManager(colliderManager);
    
    colliderManager->AddCollider(platformCollider);
    colliderManager->AddCollider(cubeCollider);
    colliderManager->AddCollider(cube2Collider);
    
    float time = 0.0f;
    
    while (!window->ShouldClose()) {
        Time::Tick();
        time += Time::DeltaTime();
        
        input.Update();
        
        float cameraSpeed = 5.0f * Time::DeltaTime();
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
        
        if (pointLight1 && pointLight2) {
            pointLight1->SetPosition(Vector3(
                3.0f * cos(time),
                3.0f + sin(time * 2.0f) * 1.0f,
                3.0f * sin(time)
            ));
            
            pointLight2->SetPosition(Vector3(
                -3.0f + cos(time * 1.5f) * 2.0f,
                2.0f + sin(time * 1.5f) * 1.0f,
                -3.0f + sin(time * 1.5f) * 2.0f
            ));
        }
        
        if (input.GetKey(Keys::Up)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(0, 0.1f, 0));
        }
        if (input.GetKey(Keys::Down)) {
            dirLight->SetDirection(dirLight->GetDirection() - Vector3(0, 0.1f, 0));
        }
        if (input.GetKey(Keys::Left)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(-0.1f, 0, 0));
        }
        if (input.GetKey(Keys::Right)) {
            dirLight->SetDirection(dirLight->GetDirection() + Vector3(0.1f, 0, 0));
        }
        
        if (input.GetKeyDown(Keys::F)) {
            if (auto* imp = cubeBody->GetFeatureOfType<ImpulseModule>()) {
                imp->AddForce(Vector3(1, 0, 0), 10.0f);
                std::cout << "Applied force to cube! Pos: " 
                          << cube->GetComponentOfType<Transform3D>()->position << std::endl;
            }
        }
        
        static bool dirLightEnabled = true;
        if (input.GetKeyDown(Keys::L)) {
            dirLightEnabled = !dirLightEnabled;
            if (dirLightEnabled) {
                dirLight->SetIntensity(0.8f);
            } else {
                dirLight->SetIntensity(0.0f);
            }
            std::cout << "Directional light: " << (dirLightEnabled ? "ON" : "OFF") << std::endl;
        }

        if (input.GetKeyDown(Keys::P)) {
            std::cout << "Cube position: " << tCube->position << std::endl;
            std::cout << "Light1 position: " << pointLight1->GetPosition() << std::endl;
            std::cout << "Light2 position: " << pointLight2->GetPosition() << std::endl;
        }
        
        physicsWorld.Update();
        
        window->Clear();
        graphics.Update();
        window->SwapBuffers();
    }
    
    return 0;
}