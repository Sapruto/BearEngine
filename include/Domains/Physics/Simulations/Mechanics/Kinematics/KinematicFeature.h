#pragma once

#include "../../../Body/IPhysicFeature.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics {
    struct KinematicFeature : public Body::IPhysicFeature {
        Vector3f velocity;

        KinematicFeature() = default;
        KinematicFeature(const Vector3f& velocity_) : velocity(velocity_) {}
    };
}