#include "Transform3D.h"

#include "GameObject.h"
#include "HierarchySystem.h"
#include <cmath>

Transform3D::Transform3D() 
    : globalPosition(Vector3f::Zero())
    , globalScale(Vector3f::One())
    , globalRotation(Quaternionf::Identity())
    , fieldPosition("fieldPosition", Vector3f::Zero())
    , fieldScale("fieldScale", Vector3f::One())
    , fieldRotation("fieldRotation", Quaternionf::Identity())
    , dirty(true) {}

Transform3D Transform3D::combine(const Transform3D& parent) const {
    Transform3D result;
    
    Vector3f rotatedPos = parent.globalRotation.rotateVector(globalPosition);
    result.globalPosition = parent.globalPosition + rotatedPos;
    
    result.globalRotation = parent.globalRotation * globalRotation;
    result.globalRotation = result.globalRotation.normalized();
    
    result.globalScale = parent.globalScale;
    
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
                Vector3f rotatedPos = parentTransform->GetGlobalRotation().rotateVector(globalPosition);
                Vector3f worldPos = parentTransform->GetGlobalPosition() + rotatedPos;
                
                globalPosition = worldPos;
                globalRotation = parentTransform->GetGlobalRotation() * globalRotation;
                globalRotation = globalRotation.normalized();
                globalScale = parentTransform->GetGlobalScale();
            }
        }
    }
    cachedMatrix = Matrix4x4f::Identity();

    float xx = globalRotation.ratioVec.x * globalRotation.ratioVec.x;
    float yy = globalRotation.ratioVec.y * globalRotation.ratioVec.y;
    float zz = globalRotation.ratioVec.z * globalRotation.ratioVec.z;
    float xy = globalRotation.ratioVec.x * globalRotation.ratioVec.y;
    float xz = globalRotation.ratioVec.x * globalRotation.ratioVec.z;
    float yz = globalRotation.ratioVec.y * globalRotation.ratioVec.z;
    float wx = globalRotation.scalar * globalRotation.ratioVec.x;
    float wy = globalRotation.scalar * globalRotation.ratioVec.y;
    float wz = globalRotation.scalar * globalRotation.ratioVec.z;
    
    cachedMatrix(0, 0) = (1 - 2 * (yy + zz)) * globalScale.x;
    cachedMatrix(0, 1) = (2 * (xy - wz)) * globalScale.x;
    cachedMatrix(0, 2) = (2 * (xz + wy)) * globalScale.x;
    cachedMatrix(0, 3) = globalPosition.x;
    
    cachedMatrix(1, 0) = (2 * (xy + wz)) * globalScale.y;
    cachedMatrix(1, 1) = (1 - 2 * (xx + zz)) * globalScale.y;
    cachedMatrix(1, 2) = (2 * (yz - wx)) * globalScale.y;
    cachedMatrix(1, 3) = globalPosition.y;
    
    cachedMatrix(2, 0) = (2 * (xz - wy)) * globalScale.z;
    cachedMatrix(2, 1) = (2 * (yz + wx)) * globalScale.z;
    cachedMatrix(2, 2) = (1 - 2 * (xx + yy)) * globalScale.z;
    cachedMatrix(2, 3) = globalPosition.z;
    
    cachedMatrix(3, 0) = 0;
    cachedMatrix(3, 1) = 0;
    cachedMatrix(3, 2) = 0;
    cachedMatrix(3, 3) = 1;
    
    dirty = false;
}

void Transform3D::rotate(float pitch, float yaw, float roll) {
    Quaternionf qx = Quaternionf::createRotation(pitch, Vector3f(1, 0, 0));
    Quaternionf qy = Quaternionf::createRotation(yaw, Vector3f(0, 1, 0));
    Quaternionf qz = Quaternionf::createRotation(roll, Vector3f(0, 0, 1));
    globalRotation = qz * qy * qx * globalRotation;
    globalRotation = globalRotation.normalized();
    dirty = true;
}

Vector3f Transform3D::forward() const {
    return globalRotation.rotateVector(Vector3f(0, 0, 1));
}

Vector3f Transform3D::backward() const {
    return -forward();
}

Vector3f Transform3D::right() const {
    return globalRotation.rotateVector(Vector3f(1, 0, 0));
}

Vector3f Transform3D::left() const {
    return -right();
}

Vector3f Transform3D::up() const {
    return globalRotation.rotateVector(Vector3f(0, 1, 0));
}

Vector3f Transform3D::down() const {
    return -up();
}

void Transform3D::lookAt(const Vector3f& target) {
    Vector3f direction = (target - globalPosition).normalized();
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
        globalRotation.scalar = 0.25f / s;
        globalRotation.ratioVec.x = (mat(2,1) - mat(1,2)) * s;
        globalRotation.ratioVec.y = (mat(0,2) - mat(2,0)) * s;
        globalRotation.ratioVec.z = (mat(1,0) - mat(0,1)) * s;
    } 
    else {
        if (mat(0,0) > mat(1,1) && mat(0,0) > mat(2,2)) {
            float s = 2.0f * sqrtf(1.0f + mat(0,0) - mat(1,1) - mat(2,2));
            globalRotation.scalar = (mat(2,1) - mat(1,2)) / s;
            globalRotation.ratioVec.x = 0.25f * s;
            globalRotation.ratioVec.y = (mat(0,1) + mat(1,0)) / s;
            globalRotation.ratioVec.z = (mat(0,2) + mat(2,0)) / s;
        } 
        else if (mat(1,1) > mat(2,2)) {
            float s = 2.0f * sqrtf(1.0f + mat(1,1) - mat(0,0) - mat(2,2));
            globalRotation.scalar = (mat(0,2) - mat(2,0)) / s;
            globalRotation.ratioVec.x = (mat(0,1) + mat(1,0)) / s;
            globalRotation.ratioVec.y = 0.25f * s;
            globalRotation.ratioVec.z = (mat(1,2) + mat(2,1)) / s;
        } 
        else {
            float s = 2.0f * sqrtf(1.0f + mat(2,2) - mat(0,0) - mat(1,1));
            globalRotation.scalar = (mat(1,0) - mat(0,1)) / s;
            globalRotation.ratioVec.x = (mat(0,2) + mat(2,0)) / s;
            globalRotation.ratioVec.y = (mat(1,2) + mat(2,1)) / s;
            globalRotation.ratioVec.z = 0.25f * s;
        }
    }
    globalRotation = globalRotation.normalized();

    dirty = true;
}

Matrix4x4f Transform3D::GetMatrix() const {
    return cachedMatrix;
}

void Transform3D::Update() {
    bool changed = false;
    
    if (fieldPosition.GetValue().x != globalPosition.x || 
        fieldPosition.GetValue().y != globalPosition.y || 
        fieldPosition.GetValue().z != globalPosition.z) {
        globalPosition = fieldPosition.GetValue();
        changed = true;
    }
    
    if (fieldScale.GetValue().x != globalScale.x || 
        fieldScale.GetValue().y != globalScale.y || 
        fieldScale.GetValue().z != globalScale.z) {
        globalScale = fieldScale.GetValue();
        changed = true;
    }
    
    if (fieldRotation.GetValue().scalar != globalRotation.scalar ||
        fieldRotation.GetValue().ratioVec.x != globalRotation.ratioVec.x ||
        fieldRotation.GetValue().ratioVec.y != globalRotation.ratioVec.y ||
        fieldRotation.GetValue().ratioVec.z != globalRotation.ratioVec.z) {
        globalRotation = fieldRotation.GetValue();
        changed = true;
    }

    if (changed) dirty = true;
    if (dirty) {
        updateWorld();
    }
}

const Vector3f& Transform3D::GetGlobalPosition() const {
    return globalPosition; 
}

const Vector3f& Transform3D::GetGlobalScale() const {
    return globalScale; 
}

const Quaternionf& Transform3D::GetGlobalRotation() const {
    return globalRotation; 
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