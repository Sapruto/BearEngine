#pragma once

#include <variant>

#include "Math/Vector/Vector3.h"

struct DirectionalLightData {
    Vector3f direction;
    Vector3f color;
    float intensity = 1.0f;
};

struct PointLightData {
    Vector3f position;
    Vector3f color;
    float radius = 1.0f;
    float intensity = 1.0f;
};

using LightData = std::variant<
    DirectionalLightData,
    PointLightData
>;