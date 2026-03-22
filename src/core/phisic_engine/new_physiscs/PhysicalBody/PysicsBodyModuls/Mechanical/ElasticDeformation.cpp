#include "ElasticDeformation.h"
#include "PhysicalBody.h"

ElasticDeformation::ElasticDeformation(float k, const std::vector<Vector3>& points) 
    : k(k) 
{
    for (size_t i = 0; i < points.size(); i++) {
        PointData data;
        data.localPos = points[i];
        data.currentWorld = points[i];
        data.targetWorld = points[i];
        data.isChanged = false;
        
        this->points[i] = data;
    }
}

void ElasticDeformation::ApplyTransformToBindingPoints(){
    Transform3D* transform = body->GetTransform();
    if (!transform) return;

    for (auto& [index, data] : points) {
        data.currentWorld = data.localPos + transform->position;
        
        if (!data.isChanged) {
            data.targetWorld = data.currentWorld;
        }
    }
}

void ElasticDeformation::UpdateLengthes(){
    lengthes.clear();
    
    for (auto& [index, data] : points) {
        if (data.isChanged) {
            Vector3 displacement = data.targetWorld - data.currentWorld;
            float length = displacement.magnitude();
            
            if (length > 1e-6) {
                Vector3 direction = displacement.normalized();
                lengthes.push_back({length, direction});
            }
        }
    }
    lengthesUpdated = true;
}

void ElasticDeformation::Initialize() { 
    isValid = true;
    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    if(!impulseModule) isValid = false;
}

void ElasticDeformation::UpdateBody() {
    if(!isValid || k == 0) return;

    ApplyTransformToBindingPoints();

    if(!lengthesUpdated){
        UpdateLengthes();
    }

    for(const auto& [length, direction] : lengthes){ 
        if (length > 0.0001f) {
            float forceMagnitude = k * length;
            Force elasticityForce(direction, forceMagnitude);
            impulseModule->AddForce(elasticityForce);
        }
    }

    for (auto& [index, data] : points) {
        if (data.isChanged) {
            data.isChanged = false;
        }
    }

    lengthes.clear();
    lengthesUpdated = false;
}

void ElasticDeformation::ReactionOnEvent(BasePhysicsEvent* event) {
    AbstractPhysicsData* events_data = event->GetData();

    auto* elastic_data = dynamic_cast<ElasticityDeformationData*>(events_data);
    if (!elastic_data) { 
        return; 
    }

    const auto& receivedPoints = elastic_data->GetChangedBindingPoints();
    
    for (const auto& [index, worldPos] : receivedPoints) {
        auto it = points.find(index);
        if (it != points.end()) {
            it->second.targetWorld = worldPos;
            it->second.isChanged = true;
        }
    }

    lengthesUpdated = false;
}


void ElasticDeformation::Destroy() {

}