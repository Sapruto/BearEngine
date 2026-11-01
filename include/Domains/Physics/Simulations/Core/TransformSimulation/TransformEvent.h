#pragma once

#include "../../../Core/IPhysicEvent.h"

#include "Math/Vector/Vector3.h"
#include "Math/Quaternion.h"

#include "TransformFeature.h"

namespace Physic::Transform {
    struct TransformEvent : public Events::IPhysicEvent {
        TransformFeature* feature = nullptr;
        Vector3f changePosition = Vector3f::Zero();
        Vector3f changeScale = Vector3f::Zero();
        Quaternionf changeRotation = Quaternionf::Identity();

        TransformEvent() = default;

        TransformEvent(TransformFeature* feature_,
                        const Vector3f& changePosition_,
                        const Vector3f& changeScale_,
                        const Quaternionf& changeRotation_)
            : feature(feature_),
            changePosition(changePosition_),
            changeScale(changeScale_),
            changeRotation(changeRotation_) {}

        TransformEvent(TransformFeature* feature_, const Vector3f& changePosition_)
            : feature(feature_),
            changePosition(changePosition_),
            changeScale(Vector3f::One()),
            changeRotation(Quaternionf::Identity()) {}

        TransformEvent(TransformFeature* feature_, const Quaternionf& changeRotation_)
            : feature(feature_),
            changePosition(Vector3f::Zero()),
            changeScale(Vector3f::One()),
            changeRotation(changeRotation_) {}

        TransformEvent(TransformFeature* feature_,
                        const Vector3f& changePosition_,
                        const Quaternionf& changeRotation_)
            : feature(feature_),
            changePosition(changePosition_),
            changeScale(Vector3f::One()),
            changeRotation(changeRotation_) {}
    };
}