#pragma once

#include "Vector3.h"
#include "Force.h"
#include "PhysicFeature.h"
#include "PhysicsEvents.h"
#include "PhysicsCollisionData.h"
#include "PhysicalBody.h"

#include "Newtonian.h"

struct ContactData {
    Vector3f frictionForce; 
    float contactMu;     
    float normalForce;
};

class Friction : public PhysicFeature{
private:
    float mu;
    float N;

    float defaultOtherMu = 2.0f;

    ImpulseModule* impulseModule = nullptr;

    std::vector<Vector3f> normals;
    
    std::vector<ContactData> contacts;

    bool isValid = false;

    template<typename T>
    void CalculateNormal(const T& segments) {
        if(!isValid) return;
        
        const Vector3f& bodyPosition = body->GetTransform()->GetGlobalPosition();
        
        for(const auto& segment : segments) {
            Vector3f direction = segment.getDirection().normalized();
            
            Vector3f normal;
            if (abs(direction.x) < 0.9f) {
                normal = Vector3f(1, 0, 0).cross(direction);
            } 
            else {
                normal = Vector3f(0, 1, 0).cross(direction);
            }
            normal = normal.normalized();
            
            Vector3f surfacePoint = (segment.point1 + segment.point2) * 0.5f;
            
            Vector3f toBody = bodyPosition - surfacePoint;
            
            if (normal.dot(toBody) < 0) {
                normal = -normal; 
            }

            normal = normal.normalized();
            
            normals.push_back(normal);
        }
    }

    void CalculateNormalForce();

    float GetOtherMu(GameObject* otherObject);

public:
    Friction() = default;
    Friction(float mu);

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    void SetMu(float mu){ this->mu = mu; }
    void SetDefaultOtherMu(float mu) { defaultOtherMu = mu; }
    float GetMu(){ return mu; }
};