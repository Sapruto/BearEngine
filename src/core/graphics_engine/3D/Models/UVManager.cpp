#include "UVManager.h"

#include "ModelComponent.h"
#include "Model.h"
#include "Transform3D.h"
#include "Camera3D.h"

ObjectUVData UVManager::CalculateObjectUV(ModelComponent* model) {
    ObjectUVData data;
    data.objectID = model->GetID();
    data.isVisible = false;
    
    Transform3D* transform = model->gameObject->GetComponentOfType<Transform3D>();
    if(!transform) return data;
    
    Model* mesh = model->GetModel();
    if(!mesh) return data;
    
    const auto& vertices = mesh->GetVertices();
    const auto& indices = mesh->GetIndices();
    
    for(const auto& vertex : vertices){
        data.localVertices.push_back(Vector3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
    }
    data.indices = indices;
    data.modelMatrix = transform->GetMatrix();
    data.isVisible = true;
    
    return data;
}

void UVManager::UpdateObjectUVWithCamera(ObjectUVData& data) {
    if(!camera) return;

    glm::mat4 mvp = camera->GetProjectionMatrix(aspect) * camera->GetViewMatrix() * data.modelMatrix;
    
    std::vector<Vector2> projectedVertices;
    std::vector<float> depths;
    
    for(const auto& localPos : data.localVertices){
        glm::vec4 clipSpace = mvp * glm::vec4(localPos.x, localPos.y, localPos.z, 1.0f);
        
        if(clipSpace.w <= 0.001f){
            projectedVertices.push_back(Vector2(-1, -1));
            depths.push_back(1.0f);
            continue;
        }
        
        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
        projectedVertices.push_back(Vector2(ndc.x, ndc.y));
        depths.push_back(ndc.z);
    }
    
    data.screenVertices = projectedVertices;
    data.depths = depths;
}

void UVManager::UpdateUVs(std::vector<ModelComponent*> models, Camera3D* camera, float aspect) {
    if (this->aspect <= 0.001f || aspect <= 0.001f) {
        this->aspect = 1.0f; 
    }
    else{
        this->aspect = aspect;
    }

    if(!camera) return;

    this->camera = camera;

    glm::mat4 currentView = camera->GetViewMatrix();
    glm::mat4 currentProj = camera->GetProjectionMatrix(this->aspect);
    
    if (currentView != m_LastViewMatrix || currentProj != m_LastProjectionMatrix) {
        m_LastViewMatrix = currentView;
        m_LastProjectionMatrix = currentProj;
    }
    
    for (auto* model : models) {
        if(!model) continue;

        objectUpdateFrameState[model->GetID()] = false;
    }
}

ObjectUVData UVManager::GetObjectUV(ModelComponent* model){
    if(!model || !camera) return ObjectUVData();

    int id = model->GetID();
    
    if(!objectUpdateFrameState[id]){
        ObjectUVData data = CalculateObjectUV(model);
        objectUpdateFrameState[id] = true;
        
        if (data.isVisible) {
            UpdateObjectUVWithCamera(data);
            objectUVs[id] = data;
        }
        
        return data;
    }
    
    return objectUVs[id];
}