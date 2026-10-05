#include "Mesh.h"
#include "RendererScene.h"

std::atomic<unsigned> Mesh::nextID = 0;

Mesh::Mesh(ResourceManager& resource, const std::string& modelPath, Vector3f colorRGB) 
    : rm(&resource), id(nextID.fetch_add(1))
{
    SetColor(colorRGB);

    this->modelPath.GetValue() = modelPath;

    rm->LoadResource(this->modelPath.GetValue(), ResourceType::Model);
    model = rm->GetResourceAs<Model>(this->modelPath.GetValue());
}

Mesh::~Mesh() {

}

void Mesh::Start() {
    if(!gameObject) return;

    Scene* scene = gameObject->GetScene();
    if(!scene) return;

    if (!rm) rm = scene->GetResourceManager();

    if (!rm->GetResourceAs<Model>(modelPath.GetValue())) rm->LoadResource(modelPath.GetValue(), ResourceType::Model);
    if (!model) model = rm->GetResourceAs<Model>(modelPath.GetValue());
}