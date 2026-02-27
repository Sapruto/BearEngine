#include "include\core\math\Transform2D.h"

#include "include\core\math\Vector2.h"
#include "include/core/system_engine/component_system/Component.h"

void Transform2D::rotate(float angleRad) {
    rotation += angleRad;
    while (rotation > 3.1415926535f) rotation -= 6.283185307f;
    while (rotation < -3.1415926535f) rotation += 6.283185307f;
}

Vector2 Transform2D::forward() const {
    return Vector2(cosf(rotation), sinf(rotation));
}

Vector2 Transform2D::right() const {
    return Vector2(-sinf(rotation), cosf(rotation));
}

void Transform2D::lookAt(const Vector2& target) {
    Vector2 direction = (target - position).normalized();
    rotation = atan2f(direction.y, direction.x);
}