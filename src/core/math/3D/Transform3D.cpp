#include "Transform3D.h"

#include "GameObject.h"
#include "HierarchySystem.h"
#include <cmath>

Transform3D::Transform3D() 
    : position(Vector3f::Zero())
    , scale(Vector3f::One())
    , rotation(Quaternionf::Identity())
    , fieldPosition("fieldPosition", Vector3f::Zero())
    , fieldScale("fieldScale", Vector3f::One())
    , fieldRotation("fieldRotation", Quaternionf::Identity())
    , dirty(true) {}

Transform3D Transform3D::combine(const Transform3D& parent) const {
    Transform3D result;
    
    Vector3f rotatedPos = parent.rotation.rotateVector(position);
    result.position = parent.position + rotatedPos;
    
    result.rotation = parent.rotation * rotation;
    result.rotation = result.rotation.normalized();
    
    result.scale = parent.scale;
    
    return result;
}

void Transform3D::updateWorld() {
    if (!dirty) return;
    
    HierarchySystem* system = gameObject->GetHierarchySystem();
    if (system) {
        std::vector<GameObject*> parents = system->GetParents(gameObject);
        if (!parents.empty()) {
            Transform3D* parentTransform = parents[0]->GetComponentOfType<Transform3D>();
            if (parentTransform) {
                Vector3f rotatedPos = parentTransform->GetRotation().rotateVector(position);
                Vector3f worldPos = parentTransform->GetPosition() + rotatedPos;
                
                position = worldPos;
                rotation = parentTransform->GetRotation() * rotation;
                rotation = rotation.normalized();
                scale = parentTransform->GetScale();
            }
        }
    }
    
    cachedMatrix = Matrix4x4f::Identity();
    
    cachedMatrix(0, 0) = scale.x;
    cachedMatrix(1, 1) = scale.y;
    cachedMatrix(2, 2) = scale.z;
    
    float xx = rotation.ratioVec.x * rotation.ratioVec.x;
    float yy = rotation.ratioVec.y * rotation.ratioVec.y;
    float zz = rotation.ratioVec.z * rotation.ratioVec.z;
    float xy = rotation.ratioVec.x * rotation.ratioVec.y;
    float xz = rotation.ratioVec.x * rotation.ratioVec.z;
    float yz = rotation.ratioVec.y * rotation.ratioVec.z;
    float wx = rotation.scalar * rotation.ratioVec.x;
    float wy = rotation.scalar * rotation.ratioVec.y;
    float wz = rotation.scalar * rotation.ratioVec.z;
    
    cachedMatrix(0, 0) = (1 - 2 * (yy + zz)) * scale.x;
    cachedMatrix(0, 1) = (2 * (xy - wz)) * scale.y;
    cachedMatrix(0, 2) = (2 * (xz + wy)) * scale.z;
    cachedMatrix(0, 3) = position.x;
    
    cachedMatrix(1, 0) = (2 * (xy + wz)) * scale.x;
    cachedMatrix(1, 1) = (1 - 2 * (xx + zz)) * scale.y;
    cachedMatrix(1, 2) = (2 * (yz - wx)) * scale.z;
    cachedMatrix(1, 3) = position.y;
    
    cachedMatrix(2, 0) = (2 * (xz - wy)) * scale.x;
    cachedMatrix(2, 1) = (2 * (yz + wx)) * scale.y;
    cachedMatrix(2, 2) = (1 - 2 * (xx + yy)) * scale.z;
    cachedMatrix(2, 3) = position.z;
    
    dirty = false;
}

void Transform3D::rotate(float pitch, float yaw, float roll) {
    Quaternionf qx = Quaternionf::createRotation(pitch, Vector3f(1, 0, 0));
    Quaternionf qy = Quaternionf::createRotation(yaw, Vector3f(0, 1, 0));
    Quaternionf qz = Quaternionf::createRotation(roll, Vector3f(0, 0, 1));
    rotation = qz * qy * qx * rotation;
    rotation = rotation.normalized();
    dirty = true;
}

Vector3f Transform3D::forward() const {
    return rotation.rotateVector(Vector3f(0, 0, 1));
}

Vector3f Transform3D::backward() const {
    return -forward();
}

Vector3f Transform3D::right() const {
    return rotation.rotateVector(Vector3f(1, 0, 0));
}

Vector3f Transform3D::left() const {
    return -right();
}

Vector3f Transform3D::up() const {
    return rotation.rotateVector(Vector3f(0, 1, 0));
}

Vector3f Transform3D::down() const {
    return -up();
}

void Transform3D::lookAt(const Vector3f& target) {
    Vector3f direction = (target - position).normalized();
    if (direction.magnitude() < 0.0001f) return;
    
    Vector3f up = Vector3f(0, 1, 0);
    Vector3f z = direction;
    Vector3f x = up.cross(z).normalized();
    Vector3f y = z.cross(x).normalized();
    
    Matrix4x4f mat;
    mat(0,0) = x.x; mat(0,1) = y.x; mat(0,2) = z.x; mat(0,3) = 0;
    mat(1,0) = x.y; mat(1,1) = y.y; mat(1,2) = z.y; mat(1,3) = 0;
    mat(2,0) = x.z; mat(2,1) = y.z; mat(2,2) = z.z; mat(2,3) = 0;
    mat(3,0) = 0;   mat(3,1) = 0;   mat(3,2) = 0;   mat(3,3) = 1;
    
    float trace = mat(0,0) + mat(1,1) + mat(2,2);
    if (trace > 0) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        rotation.scalar = 0.25f / s;
        rotation.ratioVec.x = (mat(2,1) - mat(1,2)) * s;
        rotation.ratioVec.y = (mat(0,2) - mat(2,0)) * s;
        rotation.ratioVec.z = (mat(1,0) - mat(0,1)) * s;
    } 
    else {
        if (mat(0,0) > mat(1,1) && mat(0,0) > mat(2,2)) {
            float s = 2.0f * sqrtf(1.0f + mat(0,0) - mat(1,1) - mat(2,2));
            rotation.scalar = (mat(2,1) - mat(1,2)) / s;
            rotation.ratioVec.x = 0.25f * s;
            rotation.ratioVec.y = (mat(0,1) + mat(1,0)) / s;
            rotation.ratioVec.z = (mat(0,2) + mat(2,0)) / s;
        } 
        else if (mat(1,1) > mat(2,2)) {
            float s = 2.0f * sqrtf(1.0f + mat(1,1) - mat(0,0) - mat(2,2));
            rotation.scalar = (mat(0,2) - mat(2,0)) / s;
            rotation.ratioVec.x = (mat(0,1) + mat(1,0)) / s;
            rotation.ratioVec.y = 0.25f * s;
            rotation.ratioVec.z = (mat(1,2) + mat(2,1)) / s;
        } 
        else {
            float s = 2.0f * sqrtf(1.0f + mat(2,2) - mat(0,0) - mat(1,1));
            rotation.scalar = (mat(1,0) - mat(0,1)) / s;
            rotation.ratioVec.x = (mat(0,2) + mat(2,0)) / s;
            rotation.ratioVec.y = (mat(1,2) + mat(2,1)) / s;
            rotation.ratioVec.z = 0.25f * s;
        }
    }
    rotation = rotation.normalized();
    dirty = true;
}

Matrix4x4f Transform3D::GetMatrix() const {
    return cachedMatrix;
}

void Transform3D::Update() {
    bool changed = false;
    
    if (fieldPosition.GetValue().x != position.x || 
        fieldPosition.GetValue().y != position.y || 
        fieldPosition.GetValue().z != position.z) {
        position = fieldPosition.GetValue();
        changed = true;
    }
    
    if (fieldScale.GetValue().x != scale.x || 
        fieldScale.GetValue().y != scale.y || 
        fieldScale.GetValue().z != scale.z) {
        scale = fieldScale.GetValue();
        changed = true;
    }
    
    if (fieldRotation.GetValue().scalar != rotation.scalar ||
        fieldRotation.GetValue().ratioVec.x != rotation.ratioVec.x ||
        fieldRotation.GetValue().ratioVec.y != rotation.ratioVec.y ||
        fieldRotation.GetValue().ratioVec.z != rotation.ratioVec.z) {
        rotation = fieldRotation.GetValue();
        changed = true;
    }
    
    if (changed) dirty = true;
    if (dirty) updateWorld();
}

const Vector3f& Transform3D::GetPosition() const {
    return position; 
}

const Vector3f& Transform3D::GetScale() const {
    return scale; 
}

const Quaternionf& Transform3D::GetRotation() const {
    return rotation; 
}

void Transform3D::SetPosition(const Vector3f& newWorldPos) { 
    if (position == newWorldPos) return;
    position = newWorldPos;
    dirty = true;
}

void Transform3D::SetScale(const Vector3f& newWorldScale) { 
    if (scale == newWorldScale) return;
    scale = newWorldScale;
    dirty = true;
}

void Transform3D::SetRotation(const Quaternionf& newWorldRot) { 
    if (rotation == newWorldRot) return;
    rotation = newWorldRot;
    dirty = true;
}

const Vector3f& Transform3D::GetLocalPosition() const { 
    return fieldPosition.GetValue(); 
}

const Vector3f& Transform3D::GetLocalScale() const { 
    return fieldScale.GetValue(); 
}

const Quaternionf& Transform3D::GetLocalRotation() const { 
    return fieldRotation.GetValue(); 
}

void Transform3D::SetLocalPosition(const Vector3f& newPos) { 
    fieldPosition = newPos;
    dirty = true;
}

void Transform3D::SetLocalScale(const Vector3f& newScale) { 
    fieldScale = newScale;
    dirty = true;
}

void Transform3D::SetLocalRotation(const Quaternionf& newRot) { 
    fieldRotation = newRot;
    dirty = true;
}