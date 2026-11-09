#pragma once

#include "../Core/ICollisionDetector.h"
#include "Polyhedron3D.h"

class CollisionDetector3D : public ICollisionDetector {
private:
    bool InteresectAABB(BaseCollider* a, BaseCollider* b) {
        auto aPolyhedron = static_cast<Polyhedron3D*>(a);
        auto bPolyhedron = static_cast<Polyhedron3D*>(b);

        const AABB& aAABB = aPolyhedron->GetAABB();
        const AABB& bAABB = bPolyhedron->GetAABB();

        return (aAABB.min.x <= bAABB.max.x && aAABB.max.x >= bAABB.min.x) &&
                (aAABB.min.y <= bAABB.max.y && aAABB.max.y >= bAABB.min.y) &&
                (aAABB.min.z <= bAABB.max.z && aAABB.max.z >= bAABB.min.z);
    }

    bool InteresectSAT(BaseCollider* a, BaseCollider* b) {
        auto aPolyhedron = static_cast<Polyhedron3D*>(a);
        auto bPolyhedron = static_cast<Polyhedron3D*>(b);
        if (!aPolyhedron || !bPolyhedron) return false;

        auto& aFaces = aPolyhedron->GetFaces();
        auto& bFaces = bPolyhedron->GetFaces();

        std::vector<Vector3f> allAxes;
        
        for(const auto& face : aFaces) {
            allAxes.push_back(face.normal);
        }
        
        for(const auto& face : bFaces) {
            allAxes.push_back(face.normal);
        }

        float minOverlap = std::numeric_limits<float>::max();
        Vector3f penetrationAxis = Vector3f::Zero();
        for(const auto& aEdge : aPolyhedron->GetCommunications()) {
            Vector3f aEdgeDir = aPolyhedron->GetEdgeDirection(aEdge);
            
            for(const auto& bEdge : bPolyhedron->GetCommunications()) {
                Vector3f bEdgeDir = bPolyhedron->GetEdgeDirection(bEdge);
                
                Vector3f axis = aEdgeDir.Cross(bEdgeDir);
                
                if(axis.Magnitude() > 0.0001f) {
                    axis.Normalize();
                    allAxes.push_back(axis);
                }
            }
        }

        auto transformA = aPolyhedron->GetTransform();
        auto transformB = bPolyhedron->GetTransform();
        for(const auto& axis : allAxes) {
            
            float aMin = 999999.0f;
            float aMax = -999999.0f;
            
            for(const auto& vertex : aPolyhedron->GetVertices()) {
                Vector3f global = vertex.GetGlobalPoint(*transformA);
                float proj = global.Dot(axis);
                aMin = std::min(aMin, proj);
                aMax = std::max(aMax, proj);
            }
            
            float bMin = 999999.0f;
            float bMax = -999999.0f;
            
            for(const auto& vertex : bPolyhedron->GetVertices()) {
                Vector3f global = vertex.GetGlobalPoint(*transformB);
                float proj = global.Dot(axis);
                bMin = std::min(bMin, proj);
                bMax = std::max(bMax, proj);
            }
            
            if(aMax < bMin || bMax < aMin){
                return false;
            }

            float overlap = std::min(aMax, bMax) - std::max(aMin, bMin);
            
            Vector3f direction = axis;
            
            if(aMax > bMax){
                direction = -direction;
            }
            
            if(overlap < minOverlap){
                minOverlap = overlap;
                penetrationAxis = direction * overlap;
            }
        }

        aPolyhedron->AddOverlap(
            b, 
            penetrationAxis
        );
        
        return true;
    }

public:
    bool HasCollision(BaseCollider* a, BaseCollider* b) override {
        if (!InteresectAABB(a, b)) return false;
        if (!InteresectSAT(a, b)) return false;
        return true;
    }
};