#pragma once

#include "Vector3.h"
#include "Force.h"
#include "PhysicFeature.h"
#include "PhysicsEvents.h"
#include "PhysicsCollisionData.h"
#include "PhysicalBody.h"

#include "Newtonian.h"

struct ContactData {
    Vector3 frictionForce; 
    float contactMu;     
    float normalForce;
};

class Friction : public PhysicFeature{
private:
    float mu;
    float N;

    float defaultOtherMu = 2.0f;

    ImpulseModule* impulseModule = nullptr;

    std::vector<Vector3> normals;
    
    std::vector<ContactData> contacts;

    bool isValid = false;

    template<typename T>
    void CalculateNormal(const T& segments) {
        if(!isValid) return;
        
        Vector3 bodyPosition = body->GetTransform()->position;
        
        for(const auto& segment : segments) {
            Vector3 direction = segment.getDirection().normalized();
            
            Vector3 normal;
            if (abs(direction.x) < 0.9f) {
                normal = Vector3(1, 0, 0).cross(direction);
            } 
            else {
                normal = Vector3(0, 1, 0).cross(direction);
            }
            normal = normal.normalized();
            
            Vector3 surfacePoint = (segment.point1 + segment.point2) * 0.5f;
            
            Vector3 toBody = bodyPosition - surfacePoint;
            
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
    Friction(float mu);

    void Initialize() override;

    void ChangeBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    void SetMu(float mu){ this->mu = mu; }
    void SetDefaultOtherMu(float mu) { defaultOtherMu = mu; }
    float GetMu(){ return mu; }
};