#include "PointLight3D.h"

glm::mat4 PointLight3D::GetLightSpaceMatrix(float aspect) const{
    return glm::mat4(1.0f);
}