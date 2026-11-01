#pragma once

#include "../../../Core/IPhysicEvent.h"
#include "KinematicFeature.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics {
    struct KinematicEvent : public Events::IPhysicEvent {
        KinematicFeature* feature;
        Vector3f acceleration;

        KinematicEvent() = default;
        KinematicEvent(const Vector3f& acceleration_) : acceleration(acceleration_) {}
    };
}