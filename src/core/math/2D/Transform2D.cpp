#include "Transform2D.h"

#include "GameObject.h"
#include "HierarchySystem.h"
#include <cmath>

Transform2D::Transform2D() 
    : position(Vector2f::Zero())
    , scale(Vector2f::One())
    , rotation(Quaternionf::Identity())
    , fieldPosition("fieldPosition", Vector2f::Zero())
    , fieldScale("fieldScale", Vector2f::One())
    , fieldRotation("fieldRotation", 0.0f)
    , dirty(true) {}

Transform2D Transform2D::combine(const Transform2D& parent) const {
    Transform2D result;

    Vector3f rotatedPos = parent.rotation.rotateVector(Vector3f(position.x, position.y, 0));
    result.position = parent.position + Vector2f(rotatedPos.x, rotatedPos.y);

    result.rotation = parent.rotation * rotation;
    result.rotation = result.rotation.normalized();

    result.scale = parent.scale;

    return result;
}

void Transform2D::updateWorld() {
    if (!dirty) return;
    
    HierarchySystem* system = const_cast<HierarchySystem*>(gameObject->GetHierarchySystem());
    if (system) {
        std::vector<GameObject*> parents = system->GetParents(gameObject);
        if (!parents.empty()) {
            Transform2D* parentTransform = parents[0]->GetComponentOfType<Transform2D>();
            if (parentTransform) {
                Vector3f localPos = Vector3f(position.x, position.y, 0);
                
                Vector3f parentPos = Vector3f(parentTransform->GetPosition().x, 
                                               parentTransform->GetPosition().y, 0);
                
                Vector3f rotatedPos = parentTransform->GetRotation().rotateVector(localPos);
                Vector3f worldPos = parentPos + rotatedPos;
                
                position = Vector2f(worldPos.x, worldPos.y);
                rotation = parentTransform->GetRotation() * rotation;
                rotation = rotation.normalized();
                scale = parentTransform->GetScale();
            }
        }
    }
    
    cachedMatrix = Matrix4x4f::Identity();
    cachedMatrix(0, 3) = position.x;
    cachedMatrix(1, 3) = position.y;
    
    float angle = QuaternionfToAngle(rotation);
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    cachedMatrix(0, 0) = cosA * scale.x;
    cachedMatrix(0, 1) = -sinA * scale.y;
    cachedMatrix(1, 0) = sinA * scale.x;
    cachedMatrix(1, 1) = cosA * scale.y;
    
    dirty = false;
}

float Transform2D::QuaternionfToAngle(const Quaternionf& q) const {
    return 2.0f * std::atan2(q.ratioVec.z, q.scalar);
}

Quaternionf Transform2D::angleToQuaternionf(float angle) const {
    return Quaternionf::createRotation(angle, Vector3f(0, 0, 1));
}

void Transform2D::rotate(float angleRad) {
    Quaternionf q = Quaternionf::createRotation(angleRad, Vector3f(0, 0, 1));
    rotation = q * rotation;
    rotation = rotation.normalized();
    dirty = true;
}

Vector2f Transform2D::forward() const {
    Vector3f fwd = rotation.rotateVector(Vector3f(0, 1, 0));
    return Vector2f(fwd.x, fwd.y);
}

Vector2f Transform2D::right() const {
    Vector3f rgt = rotation.rotateVector(Vector3f(1, 0, 0));
    return Vector2f(rgt.x, rgt.y);
}

void Transform2D::lookAt(const Vector2f& target) {
    Vector2f direction = (target - position).normalized();
    if (direction.magnitude() < 0.0001f) return;
    
    float angle = std::atan2(direction.x, direction.y);
    rotation = Quaternionf::createRotation(angle, Vector3f(0, 0, 1));
    rotation = rotation.normalized();
    dirty = true;
}

Matrix4x4f Transform2D::GetMatrix() const {
    return cachedMatrix;
}

float Transform2D::GetAngle() const {
    return QuaternionfToAngle(rotation);
}

void Transform2D::Update() {
    bool changed = false;
    if (fieldPosition.GetValue() != position) {
        position = fieldPosition.GetValue();
        changed = true;
    }
    if (fieldScale.GetValue() != scale) {
        scale = fieldScale.GetValue();
        changed = true;
    }
    if (std::abs(fieldRotation.GetValue() - QuaternionfToAngle(rotation)) > 0.0001f) {
        rotation = angleToQuaternionf(fieldRotation.GetValue());
        changed = true;
    }
    
    if (changed) dirty = true;
    
    if (dirty) {
        fieldRotation = QuaternionfToAngle(rotation);
        updateWorld();
    }
}

const Vector2f& Transform2D::GetPosition() const {
    return position; 
}

const Vector2f& Transform2D::GetScale() const {
    return scale; 
}

const Quaternionf& Transform2D::GetRotation() const {
    return rotation; 
}

void Transform2D::SetPosition(const Vector2f& newWorldPos) { 
    position = newWorldPos;
    fieldPosition = position;
    dirty = true;
}

void Transform2D::SetScale(const Vector2f& newWorldScale) { 
    scale = newWorldScale;
    fieldScale = scale;
    dirty = true;
}

void Transform2D::SetRotation(const Quaternionf& newWorldRot) { 
    rotation = newWorldRot;
    fieldRotation = QuaternionfToAngle(rotation);
    dirty = true;
}

void Transform2D::SetAngle(float newWorldAngle) { 
    rotation = angleToQuaternionf(newWorldAngle);
    fieldRotation = newWorldAngle;
    dirty = true;
}

const Vector2f& Transform2D::GetLocalPosition() const { 
    return fieldPosition.GetValue(); 
}

const Vector2f& Transform2D::GetLocalScale() const { 
    return fieldScale.GetValue(); 
}

float Transform2D::GetLocalAngle() const { 
    return fieldRotation.GetValue(); 
}

const Quaternionf& Transform2D::GetLocalRotation() const { 
    static Quaternionf q;
    q = angleToQuaternionf(fieldRotation.GetValue());
    return q;
}

void Transform2D::SetLocalPosition(const Vector2f& newPos) { 
    fieldPosition = newPos;
    dirty = true;
}

void Transform2D::SetLocalScale(const Vector2f& newScale) { 
    fieldScale = newScale;
    dirty = true;
}

void Transform2D::SetLocalAngle(float newAngle) { 
    fieldRotation = newAngle;
    dirty = true;
}

void Transform2D::SetLocalRotation(const Quaternionf& newRot) {
    fieldRotation = QuaternionfToAngle(newRot);
    dirty = true;
}