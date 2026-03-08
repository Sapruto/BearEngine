#pragma once

#include "Vector3.h"
#include <cmath>
#include "Transform.h"

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
    
private:
    void normalizeAngles();
};