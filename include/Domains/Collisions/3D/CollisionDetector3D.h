#pragma once

#include "../Core/ICollisionDetector.h"
#include "../Core/IShape.h"
#include "IShape3D.h"
#include "Polyhedron3D.h"
#include "ShapeRegistry.h"

#include <algorithm>

struct CollisionInfo {
    IShape3D* a;
    IShape3D* b;
    Vector3f penetration;
};

class CollisionDetector3D : public ICollisionDetector {
private:
    std::vector<CollisionInfo> collisions;

    bool IntersectAABB(IShape3D* aPolyhedron, IShape3D* bPolyhedron) {
        if (!aPolyhedron || !bPolyhedron) return false;

        const AABB_3D& aAABB = aPolyhedron->GetGlobalAABB();
        const AABB_3D& bAABB = bPolyhedron->GetGlobalAABB();

        return (aAABB.min.x <= bAABB.max.x && aAABB.max.x >= bAABB.min.x) &&
                (aAABB.min.y <= bAABB.max.y && aAABB.max.y >= bAABB.min.y) &&
                (aAABB.min.z <= bAABB.max.z && aAABB.max.z >= bAABB.min.z);
    }

    bool IntersectSAT(Polyhedron3D* aPolyhedron, Polyhedron3D* bPolyhedron) {
        if (!aPolyhedron || !bPolyhedron) return false;
        if (aPolyhedron->GetGlobalVertices().empty() ||
            bPolyhedron->GetGlobalVertices().empty()) {
            return false;
        }

        std::vector<Vector3f> allAxes;
        
        const auto& aNormals = aPolyhedron->GetGlobalFaceNormals();
        const auto& bNormals = bPolyhedron->GetGlobalFaceNormals();
        for (const auto& n : aNormals) if (n.Magnitude() > 1e-4f) allAxes.push_back(n);
        for (const auto& n : bNormals) if (n.Magnitude() > 1e-4f) allAxes.push_back(n);

        float minOverlap = std::numeric_limits<float>::max();
        Vector3f penetrationAxis = Vector3f::Zero();
        const auto& aEdges = aPolyhedron->GetGlobalEdgeDirections();
        const auto& bEdges = bPolyhedron->GetGlobalEdgeDirections();
        for (const auto& da : aEdges) {
            for (const auto& db : bEdges) {
                Vector3f axis = da.Cross(db);
                float len = axis.Magnitude();
                if (len > 1e-4f) allAxes.push_back(axis / len);
            }
        }

        for(const auto& axis : allAxes) {
            
            float aMin = std::numeric_limits<float>::max();
            float aMax = std::numeric_limits<float>::lowest();
            
            for(const auto& vertex : aPolyhedron->GetGlobalVertices()) {
                float proj = vertex.localPoint.Dot(axis);
                aMin = std::min(aMin, proj);
                aMax = std::max(aMax, proj);
            }
            
            float bMin = std::numeric_limits<float>::max();
            float bMax = std::numeric_limits<float>::lowest();
            
            for(const auto& vertex : bPolyhedron->GetGlobalVertices()) {
                float proj = vertex.localPoint.Dot(axis);
                bMin = std::min(bMin, proj);
                bMax = std::max(bMax, proj);
            }
            
            if (aMax < bMin || bMax < aMin) return false;

            float overlap = std::min(aMax, bMax) - std::max(aMin, bMin);
            
            Vector3f direction = axis;
            if ((aPolyhedron->GetGlobalCenter() - bPolyhedron->GetGlobalCenter()).Dot(direction) < 0) {
                direction = -direction;
            }
            
            if(overlap < minOverlap){
                minOverlap = overlap;
                penetrationAxis = direction * overlap;
            }
        }

        collisions.erase(
            std::remove_if(collisions.begin(), collisions.end(),
                [aPolyhedron, bPolyhedron](const CollisionInfo& c) {
                    return (c.a == aPolyhedron && c.b == bPolyhedron) ||
                        (c.a == bPolyhedron && c.b == aPolyhedron);
                }),
            collisions.end());

        collisions.push_back({
            aPolyhedron,
            bPolyhedron,
            penetrationAxis
        });
        
        return true;
    }

public:
    explicit CollisionDetector3D(ShapeRegistry& registry_) : ICollisionDetector(registry_) {}

    void ClearInfo() override {
        collisions.clear();
    }

    bool HasCollision(ShapeId a, ShapeId b) override {
        auto& registry = GetRegistry();
        auto shapeA = registry.GetShapeByType<Polyhedron3D>(a);
        auto shapeB = registry.GetShapeByType<Polyhedron3D>(b);
        if (!shapeA || !shapeB) return false;

        if (!IntersectAABB(shapeA, shapeB)) return false;
        if (!IntersectSAT(shapeA, shapeB)) return false;
        return true;
    }

    std::optional<CollisionInfo> GetCollisionInfo(ShapeId a, ShapeId b) {
        for (const auto& c : collisions) {
            ShapeId ca = c.a->GetId();
            ShapeId cb = c.b->GetId();
            if ((ca == a && cb == b) || (ca == b && cb == a))
                return c;
        }
        return std::nullopt;
    }

    const std::vector<CollisionInfo>& GetCollisions() const override { return collisions; }
};