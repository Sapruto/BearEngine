#pragma once

#include "../../../Events/IPhysicEvent.h"

#include "Domains/Physics/Body/PhysicalBody.h"
#include "Math/Vector/Vector3.h"
#include "Math/Quaternion.h"

namespace Physic::Transform {
    struct TransformEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        Vector3f changePosition = Vector3f::Zero();
        Vector3f changeScale = Vector3f::Zero();
        Quaternionf changeRotation = Quaternionf::Identity();

        TransformEvent() = default;

        TransformEvent(Body::BodyId bodyId_,
                        const Vector3f& changePosition_,
                        const Vector3f& changeScale_,
                        const Quaternionf& changeRotation_)
            : bodyId(bodyId_),
            changePosition(changePosition_),
            changeScale(changeScale_),
            changeRotation(changeRotation_) {}

        TransformEvent(Body::BodyId bodyId_, const Vector3f& changePosition_)
            : bodyId(bodyId_),
            changePosition(changePosition_),
            changeScale(Vector3f::Zero()),
            changeRotation(Quaternionf::Identity()) {}

        TransformEvent(Body::BodyId bodyId_, const Quaternionf& changeRotation_)
            : bodyId(bodyId_),
            changePosition(Vector3f::Zero()),
            changeScale(Vector3f::Zero()),
            changeRotation(changeRotation_) {}

        TransformEvent(Body::BodyId bodyId_,
                        const Vector3f& changePosition_,
                        const Quaternionf& changeRotation_)
            : bodyId(bodyId_),
            changePosition(changePosition_),
            changeScale(Vector3f::Zero()),
            changeRotation(changeRotation_) {}
    };
}