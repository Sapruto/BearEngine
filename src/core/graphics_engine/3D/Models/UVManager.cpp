#include "UVManager.h"

#include "ModelComponent.h"
#include "Model.h"
#include "Transform3D.h"
#include "Camera3D.h"

ObjectUVData UVManager::CalculateObjectUV(ModelComponent* model, Camera3D* camera, float aspect) {
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

void UVManager::UpdateObjectUVWithCamera(ObjectUVData& data, Camera3D* camera, float aspect) {
    glm::mat4 mvp = camera->GetProjectionMatrix(aspect) * camera->GetViewMatrix() * data.modelMatrix;
    
    std::vector<Vector2> projectedVertices;
    
    for(const auto& localPos : data.localVertices){
        glm::vec4 clipSpace = mvp * glm::vec4(localPos.x, localPos.y, localPos.z, 1.0f);
        
        if(clipSpace.w <= 0.001f){
            projectedVertices.push_back(Vector2(-1, -1));
            continue;
        }
        
        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
        projectedVertices.push_back(Vector2(ndc.x, ndc.y));
    }
    
    data.screenVertices = projectedVertices;
}

void UVManager::UpdateUVs(std::vector<ModelComponent*> models, Camera3D* camera, float aspect) {
    glm::mat4 currentView = camera->GetViewMatrix();
    glm::mat4 currentProj = camera->GetProjectionMatrix(aspect);
    
    if (currentView != m_LastViewMatrix || currentProj != m_LastProjectionMatrix) {
        m_CameraChanged = true;
        m_LastViewMatrix = currentView;
        m_LastProjectionMatrix = currentProj;
    }
    
    for (auto* model : models) {
        if (!model) continue;
        
        auto it = objectUVs.find(model->GetID());
        Transform3D* transform = model->gameObject->GetComponentOfType<Transform3D>();
        
        bool modelChanged = false;
        if (it != objectUVs.end()) {
            if (it->second.modelMatrix != transform->GetMatrix()) {
                modelChanged = true;
            }
        }
        
        if (it == objectUVs.end() || m_CameraChanged || modelChanged || m_NeedUpdate) {
            if (it == objectUVs.end()) {
                ObjectUVData newData = CalculateObjectUV(model, camera, aspect);
                if (newData.isVisible) {
                    UpdateObjectUVWithCamera(newData, camera, aspect);
                    objectUVs[model->GetID()] = newData;
                }
            } 
            else 
            {
                it->second.modelMatrix = transform->GetMatrix();
                UpdateObjectUVWithCamera(it->second, camera, aspect);
            }
        }
    }
    
    m_CameraChanged = false;
    m_NeedUpdate = false;
}

ObjectUVData UVManager::GetObjectUV(int objectID){
    auto it = objectUVs.find(objectID);
    if(it != objectUVs.end()){
        return it->second;
    }
    return ObjectUVData();
}

ObjectUVData UVManager::GetObjectUV(ModelComponent* model){
    if(!model) return ObjectUVData();
    return GetObjectUV(model->GetID());
}