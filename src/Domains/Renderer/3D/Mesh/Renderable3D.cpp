#include "Domains/Renderer/3D/Mesh/Renderable3D.h"

#include "Systems/Resource/ResourceSystem.h"

std::atomic<unsigned> Renderable3D::nextId = 0;

Renderable3D::Renderable3D(const std::string& meshPath) 
    : id(nextId.fetch_add(1))
{
    this->meshPath = meshPath;

    GetRequire()->AddSystemByType<ResourceSystem>();
}

void Renderable3D::Awake() {
    if (gameObject) transform = gameObject->GetComponentOfType<Transform3D>();

    auto resourceSystem = GetRequire()->GetSystemByType<ResourceSystem>();
    mesh = resourceSystem->GetResourceAs<Mesh>(meshPath);
    if (!mesh) {
        resourceSystem->LoadResource(meshPath, ResourceType::Mesh);
        mesh = resourceSystem->GetResourceAs<Mesh>(meshPath);
    }
}

void Renderable3D::Start() {
    if(!gameObject) return;

    Scene* scene = gameObject->GetScene();
    if(!scene) return;

    auto resourceSystem = GetRequire()->GetSystemByType<ResourceSystem>();
    if (!resourceSystem->GetResourceAs<Mesh>(meshPath.GetValue())) resourceSystem->LoadResource(meshPath.GetValue(), ResourceType::Mesh);
    if (!mesh) mesh = resourceSystem->GetResourceAs<Mesh>(meshPath.GetValue());
}