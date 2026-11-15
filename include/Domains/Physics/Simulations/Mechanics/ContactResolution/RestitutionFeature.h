#pragma once

#include "Domains/Physics/Body/IPhysicFeature.h"

namespace Physic::Mechanics::Contact {
    struct RestitutionFeature : public Body::IPhysicFeature {
        float restitution = 0.3f;

        RestitutionFeature() = default;
        RestitutionFeature(float restitution_) : restitution(restitution_) {}
    };
}