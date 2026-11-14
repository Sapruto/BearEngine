#pragma once

#include "../Vector/BaseVector.h"
#include "../Vector/Vector3.h"
#include "../Transform/Transform.h"
#include <type_traits>

template<typename VectorT>
    requires (std::is_base_of_v<Vector, VectorT>)
struct Vertex {
    VectorT localPoint = VectorT::Zero();
    
    VectorT GetGlobalPoint(Transform<VectorT>* transform) const {
        if (!transform) return localPoint;
        return transform->GetGlobalRotation() * localPoint + transform->GetGlobalPosition();
    }
    
    Vertex() = default;
    explicit Vertex(const VectorT& point) : localPoint(point) {}
};

using Vertex3D = Vertex<Vector3f>;