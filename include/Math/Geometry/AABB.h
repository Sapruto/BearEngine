#pragma once

#include "../Vector/BaseVector.h"
#include "../Vector/Vector3.h"
#include <type_traits>

template<typename VectorT>
    requires (std::is_base_of_v<Vector, VectorT>)
struct AABB {
    VectorT min = VectorT::Zero();
    VectorT max = VectorT::Zero();
    
    AABB() = default;
    explicit AABB(const VectorT& min_, const VectorT& max_) : min(min_), max(max_) {}
};

using AABB_3D = AABB<Vector3f>;