#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstddef>
#include "Collider.h"
#include "BaseCollider.h"
#include "Transform3D.h"
#include "Vector3.h"

#include "Vertex3D.h"

#include "Tetrahedron3D.h"

struct Segment3D {
    Vector3f point1;
    Vector3f point2;
    
    Segment3D() : point1(), point2() {}
    Segment3D(const Vector3f& p1, const Vector3f& p2) : point1(p1), point2(p2) {}
    
    Vector3f getDirection() const { return point2 - point1; }
    float length() const { return point1.distanceTo(point2); }
    float sqrLength() const { return (point2 - point1).sqrMagnitude(); }
};

struct AABB{
    AABB() : min(Vector3f(0,0,0)), max(Vector3f(0,0,0)) {}
    AABB(const Vector3f& min, const Vector3f& max) : min(min), max(max) {}
    
    Vector3f min;
    Vector3f max;
};

struct Face {
    Face() = default;
    Face(std::vector<size_t> indices) : vertexIndices(indices) {}

    std::vector<size_t> vertexIndices;
    Vector3f normal;        
    
    void CalculateNormal(const std::vector<Vertex3D>& allVertices) {
        if (vertexIndices.size() < 3) return;
        
        Vector3f normal_sum(0, 0, 0);
        
        for (size_t i = 0; i < vertexIndices.size(); i++) {
            size_t current = vertexIndices[i];
            size_t next = vertexIndices[(i + 1) % vertexIndices.size()];
            
            const Vector3f& v_curr = allVertices[current].localPoint;
            const Vector3f& v_next = allVertices[next].localPoint;
            
            normal_sum.x += (v_curr.y - v_next.y) * (v_curr.z + v_next.z);
            normal_sum.y += (v_curr.z - v_next.z) * (v_curr.x + v_next.x);
            normal_sum.z += (v_curr.x - v_next.x) * (v_curr.y + v_next.y);
        }
        
        normal = normal_sum.normalized();
    }
};


class Polyhedron3D : public Collider<Transform3D, Vector3f> {
private:
    AABB cachedAABB;

    std::vector<Tetrahedron3D> tetrahedrons;

    Vector3f center;

    void ApplyChanged();

    Vector3f GetEdgeDirection(const std::pair<size_t, size_t>& edge) const;

    size_t FindNextVertex(size_t fromVertex, size_t notThisVertex);

    bool IntersectAABB(const Polyhedron3D* other) const;
    bool IntersectSAT(const Polyhedron3D* other) const; 

    bool CheckExactIntersection(const Polyhedron3D* other) const;

protected:
    std::vector<Vertex3D> vertices;
    std::vector<Face> faces;
    std::vector<std::pair<size_t, size_t>> communications;
    bool isChanged;
    
    void CalculateCenter();
    void CalculateAABB();
    void CalculateFaces();
    void CalculateTetrahedrons();

public:
    explicit Polyhedron3D(std::vector<Vector3f> newVertices);

    Polyhedron3D() = default;
    ~Polyhedron3D();

    void Start() override;
    void Update() override;

    float GetVolume() const override;
    
    bool ContainsPoint(const Vector3f& point) const override;
    bool Intersects(const BaseCollider* other) const override;
    Vector3f GetCenter() const override;
    bool GetCollisionInfo(const BaseCollider* other, Vector3f& point, Vector3f& normal, float& penetration) const override{}

    void AddVertex(const Vector3f& newVertex);
    void AddVertex(const Vector3f& newVertex, std::vector<size_t> vertexCommunications);

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