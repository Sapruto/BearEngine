#include "GraphicsManager.h"
#include "Window.h"
#include "Camera3D.h"
#include "Model.h"
#include "ModelComponent.h"
#include "ModelRenderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Transform3D.h"

int main() {
    // 1. Окно
    float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    Window* window = new Window(1280, 720, clearColor, "3D Test");
    if (!window->Initialize()) return -1;

    // 2. GraphicsManager
    GraphicsManager graphics;
    graphics.SetWindow(window);
    
    // Камера
    Camera3D* camera = new Camera3D(Vector3(0, 5, 30));
    graphics.SetCamera(camera);
    
    // 3. ResourceManager
    ResourceManager resources;
    
    // 4. Загружаем модель
    resources.LoadResource("Assets/models/cube.obj", ResourceType::Model);
    Model* cubeModel = resources.GetResourceAs<Model>("Assets/models/cube.obj");
    
    if (!cubeModel || !cubeModel->IsLoaded()) {
        std::cout << "Failed to load model!" << std::endl;
        return -1;
    }
    
    // 5. Рендерер
    ModelRenderer* renderer = graphics.AddRender<ModelRenderer>();
    std::cout << "Renderer address: " << renderer << std::endl;
    
    // 6. Цвета
    float red[3] = {1, 0, 0};
    float green[3] = {0, 1, 0};
    float blue[3] = {0, 0, 1};
    float yellow[3] = {1, 1, 0};
    
    // 7. Создаем объекты - РАЗЫМЕНОВЫВАЕМ renderer!
    GameObject* obj1 = new GameObject();
    Transform3D* t1 = obj1->AddComponent<Transform3D>();
    t1->position = Vector3(-3, 0, 0);
    t1->scale = Vector3(0.1f, 0.1f, 0.1f);
    ModelComponent* m1 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", red);
    obj1->AddComponent(m1);
    
    GameObject* obj2 = new GameObject();
    Transform3D* t2 = obj2->AddComponent<Transform3D>();
    t2->position = Vector3(3, 0, 0);
    t2->scale = Vector3(0.1f, 0.1f, 0.1f);
    ModelComponent* m2 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", green);
    obj2->AddComponent(m2);
    
    GameObject* obj3 = new GameObject();
    Transform3D* t3 = obj3->AddComponent<Transform3D>();
    t3->position = Vector3(0, 2, 0);
    t3->scale = Vector3(0.1f, 0.1f, 0.1f);
    ModelComponent* m3 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", blue);
    obj3->AddComponent(m3);
    
    GameObject* obj4 = new GameObject();
    Transform3D* t4 = obj4->AddComponent<Transform3D>();
    t4->position = Vector3(0, -1, 2);
    t4->scale = Vector3(0.1f, 0.1f, 0.1f);
    ModelComponent* m4 = new ModelComponent(resources, *renderer, "Assets/models/cube.obj", yellow);
    obj4->AddComponent(m4);

    renderer->RegisterRenderComponent(m1);
    renderer->RegisterRenderComponent(m2);
    renderer->RegisterRenderComponent(m3);
    renderer->RegisterRenderComponent(m4);
    
    // 8. Главный цикл
    while (!window->ShouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        t4->rotation.y += 0.5f;
        
        graphics.Update();
        
        window->SwapBuffers();
        window->PollEvents();
    }
    
    return 0;
}