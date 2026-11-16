#include "Domains/Renderer/3D/Settings/Lights/PointLight3D.h"

#include "Math/Transform/Transform3D.h"

using namespace Renderer::R3D::Lights;

LightData PointLight3D::GetLightData() const {
    PointLightData data;
    data.position = GetGlobalPosition();
    data.color = GetColor();
    data.intensity = GetIntensity();
    data.radius = GetRadius();
    return data;
}