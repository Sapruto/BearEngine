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

    std::vector<float> depths;
    
    ObjectUVData() : objectID(-1), isVisible(false), modelMatrix(1.0f) {}
};

class ModelComponent;
class Camera3D;

class UVManager {
private:
    std::unordered_map<int, ObjectUVData> objectUVs;
    std::unordered_map<int, bool> objectUpdateFrameState;

    glm::mat4 m_LastViewMatrix;
    glm::mat4 m_LastProjectionMatrix;

    Camera3D* camera = nullptr;
    float aspect = 1.0f;
    
    ObjectUVData CalculateObjectUV(ModelComponent* model);
    void UpdateObjectUVWithCamera(ObjectUVData& data);
    
public:
    UVManager() = default;
    
    void UpdateUVs(std::vector<ModelComponent*> models, Camera3D* camera, float aspect);
    
    ObjectUVData GetObjectUV(ModelComponent* model);
    
    const std::unordered_map<int, ObjectUVData>& GetAllUVs() const { return objectUVs; }
};