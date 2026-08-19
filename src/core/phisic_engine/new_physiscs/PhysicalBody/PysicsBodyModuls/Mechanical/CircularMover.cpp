#include "CircularMover.h"

#include "Newtonian.h"

CircularMover::CircularMover(const Vector3& anchorPoint, 
    const Vector3& direction, 
    float radius,
    float periudRotate,    
    float stiffness,
    float accelerationFactor) :
    anchorPoint(anchorPoint), 
    direction(direction),
    radius(radius),
    periudRotate(periudRotate),
    stiffness(stiffness),
    accelerationFactor(accelerationFactor)

{ 
    SetLayer(5);
}

void CircularMover::Initialize(){
    isValid = true;
    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    float mass = body->GetMass();

    if(!impulseModule || mass == 0){
        isValid = false;
        return;
    }
}

void CircularMover::UpdateBody() {
    if(!isValid) return;

    const Vector3& currentPos = body->GetTransform()->GetGlobalPosition();
    Vector3 fromCenter = currentPos - anchorPoint;
    float distance = fromCenter.magnitude();
    
    if (distance < 0.001f) {
        Vector3 safeDir(1, 0, 0);
        body->GetTransform()->SetLocalPosition(anchorPoint + safeDir * radius);
        fromCenter = safeDir * radius;
        distance = radius;
    }
    
    Vector3 radialDir = fromCenter.normalized();
    
    if (fabs(distance - radius) > 0.1f) {
        Vector3 correctionForce = radialDir * (radius - distance) * stiffness;
        impulseModule->AddForce(correctionForce, correctionForce.magnitude());
    }
    
    Vector3 vel = impulseModule->GetVelocity();
    float radialSpeed = vel.dot(radialDir);
    Vector3 tangentialVel = vel - radialDir * radialSpeed;
    float tangentialSpeed = tangentialVel.magnitude();
    
    float centripetalForceMagnitude = body->GetMass() * tangentialSpeed * tangentialSpeed / distance;
    impulseModule->AddForce(-radialDir * centripetalForceMagnitude, (-radialDir * centripetalForceMagnitude).magnitude());
    
    Vector3 tangentDir = (direction - radialDir * direction.dot(radialDir)).normalized();
    
    if (tangentDir.magnitude() < 0.001f) {
        tangentDir = radialDir.cross(direction).normalized();
        
        if (tangentDir.magnitude() < 0.001f) {
            Vector3 worldUp(0, 1, 0);
            tangentDir = radialDir.cross(worldUp).normalized();
            
            if (tangentDir.magnitude() < 0.001f) {
                Vector3 worldRight(1, 0, 0);
                tangentDir = radialDir.cross(worldRight).normalized();
            }
        }
    }
    
    constexpr float PI = 3.14159265358979323846f;
    float targetSpeed = 2.0f * PI * radius / periudRotate;
    float speedDiff = targetSpeed - tangentialSpeed;
    
    float forceMagnitude = speedDiff * body->GetMass() * accelerationFactor; 
    impulseModule->AddForce(tangentDir * forceMagnitude, (tangentDir * forceMagnitude).magnitude());
}