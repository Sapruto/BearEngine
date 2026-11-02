#pragma once

#include "../../../Body/IPhysicFeature.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Dynamics {
    struct DynamicFeature : public Body::IPhysicFeature {
        Vector3f force;

        DynamicFeature() = default;
        DynamicFeature(const Vector3f& force_) : force(force_) {}
    };
}