#pragma once

#include "PhysicFeature.h"
#include "PhysicsEvents.h"
#include "Force.h"
#include "Vector3.h"
#include "Transform3D.h"
#include "ElasticityDeformationData.h"

#include "Newtonian.h"

#include <unordered_map>
#include <string>

class ElasticDeformation : public PhysicFeature{
private:
    float k = 0;

    ImpulseModule* impulseModule = nullptr;

    std::vector<std::pair<float, Vector3>> lengthes;

    struct PointData {
        Vector3 localPos;    
        Vector3 currentWorld; 
        Vector3 targetWorld; 
        bool isChanged;
    };
    
    std::unordered_map<int, PointData> points; 

    bool lengthesUpdated = false;

    bool isValid;

    void ApplyTransformToBindingPoints();
    void UpdateLengthes();

public:
    ElasticDeformation(float k, const std::vector<Vector3>& bindingPoints); 

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;
};