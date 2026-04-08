#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "Vector2.h"
#include "Vector3.h"

struct ObjectUVData {
    int objectID;
    std::vector<Vector3> localVertices;
    std::vector<Vector2> screenVertices;
    std::vector<unsigned int> indices;
    glm::mat4 modelMatrix;
    bool isVisible;
    
    ObjectUVData() : objectID(-1), isVisible(false), modelMatrix(1.0f) {}
};

class ModelComponent;
class Camera3D;

class UVManager {
private:
    std::unordered_map<int, ObjectUVData> objectUVs;
    bool m_NeedUpdate = true;

    glm::mat4 m_LastViewMatrix;
    glm::mat4 m_LastProjectionMatrix;
    bool m_CameraChanged = true;
    
    ObjectUVData CalculateObjectUV(ModelComponent* model, Camera3D* camera, float aspect);
    void UpdateObjectUVWithCamera(ObjectUVData& data, Camera3D* camera, float aspect);
    
public:
    UVManager() = default;
    
    void UpdateUVs(std::vector<ModelComponent*> models, Camera3D* camera, float aspect);
    
    ObjectUVData GetObjectUV(int objectID);
    ObjectUVData GetObjectUV(ModelComponent* model);
    
    void MarkForUpdate() { m_NeedUpdate = true; }
    
    const std::unordered_map<int, ObjectUVData>& GetAllUVs() const { return objectUVs; }
};