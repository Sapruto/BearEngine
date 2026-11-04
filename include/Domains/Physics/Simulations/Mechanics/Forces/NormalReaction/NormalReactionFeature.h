#pragma once

#include "Domains/Physics/Body/IPhysicFeature.h"
#include "Domains/Collisions/Core/BaseCollider.h"

namespace Physic::Mechanics::Forces::NormalReaction {
    struct NormalReactionFeature : public Body::IPhysicFeature{
        bool isGround;
        BaseCollider* collider;
    };
}