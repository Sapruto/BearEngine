#pragma once

#include "Vector3.h"
#include <cmath>
#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "HierarchySystem.h"
#include "SerializeField.h"

class Transform3D : public Transform, public ISerializable {
private:
    FIELD(Vector3, fieldPosition);
    FIELD(Vector3, fieldScale);
    FIELD(Vector3, fieldRotation);

    Vector3 position{Vector3::Zero};
    Vector3 scale{Vector3(0, 0, 0)};
    Vector3 rotation{Vector3::Zero};

public:
    Transform3D();
    
    void rotate(float pitch, float yaw, float roll);
    Vector3 forward() const;
    Vector3 backward() const;
    Vector3 right() const;
    Vector3 left() const;
    Vector3 up() const;
    Vector3 down() const;
    void lookAt(const Vector3& target);

    glm::mat4 GetMatrix() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, glm::vec3(position.x, position.y, position.z));
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        mat = glm::scale(mat, glm::vec3(scale.x, scale.y, scale.z));
        return mat;
    }

    void Update() override {
        position = fieldPosition.GetValue();
        scale = fieldScale.GetValue();
        rotation = fieldRotation.GetValue();

        HierarchySystem* system = const_cast<HierarchySystem*>(gameObject->GetHierarchySystem());
        if(!system) return;

        std::vector<GameObject*> parents = system->GetParents(gameObject);

        for(auto* parent : parents){
            Transform3D* parentTransform = parent->GetComponentOfType<Transform3D>();
            if(!parentTransform) continue;

            position = position + parentTransform->GetPosition();
            scale = scale * parentTransform->GetScale();
            rotation = rotation + parentTransform->GetRotation();
        }
    }

    const Vector3& GetPosition() const { return position; }
    const Vector3& GetScale() const { return scale; }
    const Vector3& GetRotation() const { return rotation; }

    void SetPosition(const Vector3& newWorldPos) { 
        HierarchySystem* system = const_cast<HierarchySystem*>(gameObject->GetHierarchySystem());
        if (system && !system->GetParents(gameObject).empty()) {
            Vector3 parentWorldPos = Vector3::Zero;
            for (auto* parent : system->GetParents(gameObject)) {
                Transform3D* parentTransform = parent->GetComponentOfType<Transform3D>();
                if (parentTransform) {
                    parentWorldPos = parentWorldPos + parentTransform->GetPosition();
                }
            }
            fieldPosition.GetValue() = newWorldPos - parentWorldPos;
        } 
        else {
            fieldPosition.GetValue() = newWorldPos;
        }
    }
    void SetScale(const Vector3& newWorldScale) { 
        HierarchySystem* system = const_cast<HierarchySystem*>(gameObject->GetHierarchySystem());
        if (system && !system->GetParents(gameObject).empty()) {
            Vector3 parentWorldScale = Vector3(1, 1, 1);
            for (auto* parent : system->GetParents(gameObject)) {
                Transform3D* parentTransform = parent->GetComponentOfType<Transform3D>();
                if (parentTransform) {
                    parentWorldScale = parentWorldScale * parentTransform->GetScale();
                }
            }
            fieldScale.GetValue() = Vector3(
                newWorldScale.x / parentWorldScale.x,
                newWorldScale.y / parentWorldScale.y,
                newWorldScale.z / parentWorldScale.z
            );
        } 
        else {
            fieldScale.GetValue() = newWorldScale;
        }
    }
    void SetRotation(const Vector3& newWorldRot) { 
        HierarchySystem* system = const_cast<HierarchySystem*>(gameObject->GetHierarchySystem());
        if (system && !system->GetParents(gameObject).empty()) {
            Vector3 parentWorldRot = Vector3::Zero;
            for (auto* parent : system->GetParents(gameObject)) {
                Transform3D* parentTransform = parent->GetComponentOfType<Transform3D>();
                if (parentTransform) {
                    parentWorldRot = parentWorldRot + parentTransform->GetRotation();
                }
            }
            fieldRotation.GetValue() = newWorldRot - parentWorldRot;
        } 
        else {
            fieldRotation.GetValue() = newWorldRot;
        }
    }


    const Vector3& GetLocalPosition() const { return fieldPosition.GetValue(); }
    const Vector3& GetLocalScale() const { return fieldScale.GetValue(); }
    const Vector3& GetLocalRotation() const { return fieldRotation.GetValue(); }

    void SetLocalPosition(const Vector3& newPos) {
        fieldPosition = newPos;
    }
    void SetLocalScale(const Vector3& newScale) { fieldScale = newScale; }
    void SetLocalRotation(const Vector3& newRot) { fieldRotation = newRot; }

    SERIALIZED_FIELDS(&fieldPosition, &fieldScale, &fieldRotation)
    
private:
    void normalizeAngles();
};