#pragma once

#include "Vector3.h"
#include <cmath>
#include "Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform3D : public Transform {
public:
    Vector3 position;
    Vector3 scale;
    Vector3 rotation;

    Transform3D();
    
    void rotate(float pitch, float yaw, float roll);
    Vector3 forward() const;
    Vector3 right() const;
    Vector3 up() const;
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
    
private:
    void normalizeAngles();
};