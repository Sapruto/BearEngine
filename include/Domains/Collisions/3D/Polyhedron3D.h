#pragma once

#include <vector>
#include <cstddef>
#include "IShape3D.h"
#include "Tetrahedron3D.h"
#include "Math/Vector/Vector3.h"
#include "Math/Geometry/Vertex.h"

struct Face {
    std::vector<size_t> vertexIndices;
    Vector3f normal;

    Face() = default;
    Face(std::vector<size_t> indices) : vertexIndices(indices) {}

    void CalculateNormal(const std::vector<Vertex3D>& allVertices) {
        if (vertexIndices.size() < 3) return;
        Vector3f normalSum(0, 0, 0);
        for (size_t i = 0; i < vertexIndices.size(); i++) {
            size_t current = vertexIndices[i];
            size_t next = vertexIndices[(i + 1) % vertexIndices.size()];
            const Vector3f& vCurr = allVertices[current].localPoint;
            const Vector3f& vNext = allVertices[next].localPoint;
            normalSum.x += (vCurr.y - vNext.y) * (vCurr.z + vNext.z);
            normalSum.y += (vCurr.z - vNext.z) * (vCurr.x + vNext.x);
            normalSum.z += (vCurr.x - vNext.x) * (vCurr.y + vNext.y);
        }
        normal = normalSum.Normalized();
    }
};

struct PolyhedronGeometry {
    std::vector<Vertex3D> vertices;
    std::vector<Face> faces;
    std::vector<Vector3f> faceNormals;
    std::vector<Vector3f> edgeDirections;
    Vector3f center = Vector3f::Zero();
    AABB_3D aabb;
    bool valid = false;

    void Clear() {
        vertices.clear();
        faces.clear();
        faceNormals.clear();
        edgeDirections.clear();
        center = Vector3f::Zero();
        aabb = AABB_3D(Vector3f::Zero(), Vector3f::Zero());
        valid = false;
    }
};

class Polyhedron3D : public TypedShape<Polyhedron3D, IShape3D> {
private:
    std::vector<std::pair<size_t, size_t>> communications;
    std::vector<Tetrahedron3D> tetrahedrons;

    PolyhedronGeometry localGeometry;
    mutable PolyhedronGeometry globalGeometry;

    bool isChanged;

    void ApplyChanged();

    size_t FindNextVertex(size_t fromVertex, size_t notThisVertex);

    void CalculateFaces();
    void CalculateTetrahedrons();

    void RebuildLocalGeometry();
    void RebuildGlobalGeometry() const;
    void EnsureGlobalGeometry() const;

    Vector3f ComputeLocalFaceNormal(const Face& face) const;
    Vector3f ComputeGlobalFaceNormal(const Face& face) const;

public:
    Polyhedron3D() = default;
    explicit Polyhedron3D(std::vector<Vector3f> newVertices);
    ~Polyhedron3D() = default;

    float GetVolume() const override;

    Vector3f GetCenter() const override;
    const AABB_3D& GetAABB() const override { return localGeometry.aabb; }

    Vector3f GetGlobalCenter() const override;
    const AABB_3D&  GetGlobalAABB() const override;

    const PolyhedronGeometry& GetGlobalGeometry() const;

    const std::vector<Vertex3D>& GetGlobalVertices() const {
        return GetGlobalGeometry().vertices;
    }
    const Vertex3D& GetGlobalVertex(size_t index) const {
        return GetGlobalGeometry().vertices[index];
    }
    const std::vector<Face>& GetGlobalFaces() const {
        return GetGlobalGeometry().faces;
    }
    const std::vector<Vector3f>& GetGlobalFaceNormals() const {
        return GetGlobalGeometry().faceNormals;
    }
    const std::vector<Vector3f>& GetGlobalEdgeDirections() const {
        return GetGlobalGeometry().edgeDirections;
    }

    Vector3f GetGlobalEdgeDirection(const std::pair<size_t, size_t>& edge) const;

    void AddVertex(const Vector3f& newVertex);
    void AddVertex(const Vector3f& newVertex, std::vector<size_t> vertexCommunications);
    void AddCommunication(size_t fromIndex, size_t toIndex);

    std::vector<Vertex3D>& GetVertices() { return localGeometry.vertices; }
    Vertex3D& GetVertex(size_t index) { return localGeometry.vertices[index]; }
    const std::vector<std::pair<size_t, size_t>>& GetCommunications() const { return communications; }
    std::vector<Face>& GetFaces() { return localGeometry.faces; }
    Face& GetFace(size_t index) { return localGeometry.faces[index]; }

    Vector3f GetEdgeDirection(const std::pair<size_t, size_t>& edge) const;

    const PolyhedronGeometry& GetLocalGeometry() const { return localGeometry; }

    void Update() override { EnsureGlobalGeometry(); }
};