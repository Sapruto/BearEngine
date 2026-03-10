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

struct Edge {
    size_t vertexIndex1;
    size_t vertexIndex2;
    
    Edge(size_t v1, size_t v2) : vertexIndex1(v1), vertexIndex2(v2) {}
    bool operator==(const Edge& other) const {
        return (vertexIndex1 == other.vertexIndex1 && vertexIndex2 == other.vertexIndex2) ||
               (vertexIndex1 == other.vertexIndex2 && vertexIndex2 == other.vertexIndex1);
    }
};

class Polygon3D : public Collider<Transform3D, Vector3> {
private:
    std::vector<Vector3> localVertices;
    std::vector<Edge> edges;
    mutable std::vector<Vector3> worldVerticesCache;
    mutable bool worldVerticesValid{ false };
    
    mutable Vector3 center;
    mutable bool centerValid{ false };
    
    void validateWorldVertices() const;
    void calculateCenter() const;
    
    std::vector<Segment3D> buildSegmentsFromEdges(const std::vector<Vector3>& vertices, const std::vector<Edge>& edges) const;
    Vector3 applyTransformToPoint(const Vector3& point, const Transform3D* transform) const;

    const Transform3D* GetTransform3D() const;

public:
    Polygon3D() = default;
    explicit Polygon3D(const std::vector<Vector3>& vertices);
    explicit Polygon3D(std::vector<Vector3>&& vertices);
    
    Polygon3D(const std::vector<Vector3>& vertices, const std::vector<Edge>& edges);
    Polygon3D(std::vector<Vector3>&& vertices, std::vector<Edge>&& edges);
    
    static Polygon3D FromSegments(const std::vector<Segment3D>& segments);
    static Polygon3D CreateRectangle(float width, float height, float depth = 0);
    static Polygon3D CreateConvexHull(const std::vector<Vector3>& points);
    
    void Start() override;
    void Update() override;

    float GetVolume() const override;
    
    void setVertices(const std::vector<Vector3>& vertices);
    void setVertices(std::vector<Vector3>&& vertices);
    void setEdges(const std::vector<Edge>& newEdges);
    void setEdges(std::vector<Edge>&& newEdges);
    
    void addVertex(const Vector3& vertex);
    void addEdge(size_t v1, size_t v2);
    void addEdge(const Edge& edge);
    void removeEdge(size_t v1, size_t v2);
    void removeEdge(const Edge& edge);
    void removeEdgesWithVertex(size_t vertexIndex);
    
    const std::vector<Vector3>& getLocalVertices() const { return localVertices; }
    const std::vector<Vector3>& getWorldVertices() const;
    const std::vector<Edge>& getEdges() const { return edges; }
    
    std::vector<Segment3D> getWorldSegments() const;
    std::vector<Segment3D> getLocalSegments() const;

    std::vector<Segment3D> GetIntersectionSegments(const Polygon3D& other) const;
    
    void insertVertex(size_t index, const Vector3& vertex);
    void removeVertex(size_t index);
    void clear();
    
    bool ContainsPoint(const Vector3& point) const override;
    bool Intersects(const BaseCollider* other) const override;
    Vector3 GetCenter() const override;
    bool GetCollisionInfo(const BaseCollider* other, Vector3& point, Vector3& normal, float& penetration) const override;
    
    bool intersects(const Polygon3D& other) const;
    bool intersectsFast(const Polygon3D& other) const;
    
    const Vector3& getCenter3D() const;
    size_t vertexCount() const { return localVertices.size(); }
    size_t edgeCount() const { return edges.size(); }
    bool isEmpty() const { return localVertices.empty(); }
    
    bool HasEdge(size_t v1, size_t v2) const;
    bool IsPolygonValid() const;
    std::vector<std::vector<size_t>> GetConnectedComponents() const;
    std::vector<Polygon3D> SplitIntoPolygons() const;
};