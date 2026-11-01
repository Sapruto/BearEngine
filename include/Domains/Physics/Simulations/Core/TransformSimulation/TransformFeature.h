#pragma once

#include "../../../Body/IPhysicFeature.h"
#include "Math/Vector/Vector3.h"
#include "Math/Quaternion.h"

namespace Physic::Transform {
    struct TransformFeature : public Body::IPhysicFeature {
        Vector3f position = Vector3f::Zero();
        Vector3f scale = Vector3f::Zero();
        Quaternionf rotation = Quaternionf::Identity();
    };
}