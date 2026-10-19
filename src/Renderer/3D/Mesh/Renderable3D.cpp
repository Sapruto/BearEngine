#include "Renderer/3D/Mesh/Renderable3D.h"

std::atomic<unsigned> Renderable3D::nextId = 0;

Renderable3D::Renderable3D(ResourceManager& resource, const std::string& meshPath) 
    : id(nextId.fetch_add(1)), resourceManager(&resource)
{
    this->meshPath = meshPath;
}

void Renderable3D::Awake() {
    if (gameObject) transform = gameObject->GetComponentOfType<Transform3D>();

    if (!resourceManager) return;
    mesh = resourceManager->GetResourceAs<Mesh>(meshPath);
    if (!mesh) {
        resourceManager->LoadResource(meshPath, ResourceType::Mesh);
        mesh = resourceManager->GetResourceAs<Mesh>(meshPath);
    }
}

void Renderable3D::Start() {
    if(!gameObject) return;

    Scene* scene = gameObject->GetScene();
    if(!scene) return;

    if (!resourceManager) resourceManager = scene->GetResourceManager();

    if (!resourceManager->GetResourceAs<Mesh>(meshPath.GetValue())) resourceManager->LoadResource(meshPath.GetValue(), ResourceType::Mesh);
    if (!mesh) mesh = resourceManager->GetResourceAs<Mesh>(meshPath.GetValue());
}