#pragma once

#include "Domains/Physics/Body/IPhysicFeature.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Forces::Gravity {
    struct GravityFeature : public Body::IPhysicFeature {
        Vector3f g;

        GravityFeature() = default;
        GravityFeature(const Vector3f g_) : g(g_) {}
    };
}