#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstddef>
#include "IntersectionSegments3D.h"
#include "Collider.h"
#include "BaseCollider.h"
#include "Transform3D.h"
#include "Vector3.h"

#include "Vertex3D.h"

#include "Tetrahedron3D.h"

struct AABB{
    AABB() : min(Vector3(0,0,0)), max(Vector3(0,0,0)) {}
    AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}
    
    Vector3 min;
    Vector3 max;
};

struct Face {
    Face() = default;
    Face(std::vector<size_t> indices) : vertexIndices(indices) {}

    std::vector<size_t> vertexIndices;
    Vector3 normal;        
    
    void CalculateNormal(const std::vector<Vertex3D>& allVertices) {
        if (vertexIndices.size() < 3) return;
        
        Vector3 normal_sum(0, 0, 0);
        
        for (size_t i = 0; i < vertexIndices.size(); i++) {
            size_t current = vertexIndices[i];
            size_t next = vertexIndices[(i + 1) % vertexIndices.size()];
            
            const Vector3& v_curr = allVertices[current].localPoint;
            const Vector3& v_next = allVertices[next].localPoint;
            
            normal_sum.x += (v_curr.y - v_next.y) * (v_curr.z + v_next.z);
            normal_sum.y += (v_curr.z - v_next.z) * (v_curr.x + v_next.x);
            normal_sum.z += (v_curr.x - v_next.x) * (v_curr.y + v_next.y);
        }
        
        normal = normal_sum.normalized();
    }
};


class Polyhedron3D : public Collider<Transform3D, Vector3> {
private:
    AABB cachedAABB;

    std::vector<Tetrahedron3D> tetrahedrons;

    std::vector<Vertex3D> vertices;
    std::vector<Face> faces;
    std::vector<std::pair<size_t, size_t>> communications;

    Vector3 center;

    bool isChanged;

    void ApplyChanged();

    Vector3 GetEdgeDirection(const std::pair<size_t, size_t>& edge) const;

    void CalculateTetrahedrons();
    void CalculateAABB();
    void CalculateCenter();
    void CalculateFaces();

    size_t FindNextVertex(size_t fromVertex, size_t notThisVertex);

    bool IntersectAABB(const Polyhedron3D* other) const;
    bool IntersectSAT(const Polyhedron3D* other) const; 

    bool CheckExactIntersection(const Polyhedron3D* other) const;

public:
    explicit Polyhedron3D(std::vector<Vector3> newVertices);

    ~Polyhedron3D();

    void Start() override;
    void Update() override;

    float GetVolume() const override;
    
    bool ContainsPoint(const Vector3& point) const override;
    bool Intersects(const BaseCollider* other) const override;
    Vector3 GetCenter() const override;
    bool GetCollisionInfo(const BaseCollider* other, Vector3& point, Vector3& normal, float& penetration) const override{}

    void AddVertex(const Vector3& newVertex);
    void AddVertex(const Vector3& newVertex, std::vector<size_t> vertexCommunications);

    void AddCommunication(size_t fromIndex, size_t toIndex);

    std::vector<Vertex3D>& GetVertices() { return vertices; }
    Vertex3D& GetVertex(size_t index) { return vertices[index]; }

    const std::vector<std::pair<size_t, size_t>>& GetCommunications() const { return communications; }

    std::vector<Face>& GetFaces() { return faces; }
    Face& GetFace(size_t index) { return faces[index]; }

    AABB& GetAABB() { return cachedAABB; }
    const AABB& GetAABB() const { return cachedAABB; }

    std::vector<Segment3D> GetIntersectionSegments(const Polyhedron3D& other) const;
};