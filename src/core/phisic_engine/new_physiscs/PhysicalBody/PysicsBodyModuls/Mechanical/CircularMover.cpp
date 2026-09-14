#include "CircularMover.h"

#include "Newtonian.h"

CircularMover::CircularMover(const Vector3f& anchorPoint, 
    const Vector3f& direction, 
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

    const Vector3f& currentPos = body->GetTransform()->GetGlobalPosition();
    Vector3f fromCenter = currentPos - anchorPoint;
    float distance = fromCenter.magnitude();
    
    if (distance < 0.001f) {
        Vector3f safeDir(1, 0, 0);
        body->GetTransform()->SetLocalPosition(anchorPoint + safeDir * radius);
        fromCenter = safeDir * radius;
        distance = radius;
    }
    
    Vector3f radialDir = fromCenter.normalized();
    
    if (fabs(distance - radius) > 0.1f) {
        Vector3f correctionForce = radialDir * (radius - distance) * stiffness;
        impulseModule->AddForce(correctionForce, correctionForce.magnitude());
    }
    
    Vector3f vel = impulseModule->GetVelocity();
    float radialSpeed = vel.dot(radialDir);
    Vector3f tangentialVel = vel - radialDir * radialSpeed;
    float tangentialSpeed = tangentialVel.magnitude();
    
    float centripetalForceMagnitude = body->GetMass() * tangentialSpeed * tangentialSpeed / distance;
    impulseModule->AddForce(-radialDir * centripetalForceMagnitude, (-radialDir * centripetalForceMagnitude).magnitude());
    
    Vector3f tangentDir = (direction - radialDir * direction.dot(radialDir)).normalized();
    
    if (tangentDir.magnitude() < 0.001f) {
        tangentDir = radialDir.cross(direction).normalized();
        
        if (tangentDir.magnitude() < 0.001f) {
            Vector3f worldUp(0, 1, 0);
            tangentDir = radialDir.cross(worldUp).normalized();
            
            if (tangentDir.magnitude() < 0.001f) {
                Vector3f worldRight(1, 0, 0);
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