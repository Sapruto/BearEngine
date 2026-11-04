#pragma once

#include "../../../Body/PhysicalBody.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Contact {
    struct Contact {
        Body::BodyId a;
        Body::BodyId b;
        Vector3f point;
        Vector3f normal;
        float penetration;
    };
}