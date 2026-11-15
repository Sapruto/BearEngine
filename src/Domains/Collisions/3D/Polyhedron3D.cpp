#include "Domains/Collisions/3D/Polyhedron3D.h"
#include <algorithm>
#include <cmath>
#include <set>

Polyhedron3D::Polyhedron3D(std::vector<Vector3f> newVertices) {
    for (const auto& v : newVertices) {
        localGeometry.vertices.push_back(Vertex3D(v));
    }
    isChanged = true;
    ApplyChanged();
}

void Polyhedron3D::ApplyChanged() {
    CalculateFaces();
    CalculateTetrahedrons();
    RebuildLocalGeometry();
    globalGeometry.valid = false;
    isChanged = true;
}

void Polyhedron3D::CalculateFaces() {
    auto& verts = localGeometry.vertices;
    auto& faces = localGeometry.faces;

    std::set<std::pair<size_t, size_t>> usedEdges;
    faces.clear();

    for (const auto& startEdge : communications) {
        auto edge = startEdge;
        auto reversed = std::make_pair(edge.second, edge.first);

        if (usedEdges.count(edge) > 0) continue;

        std::vector<size_t> faceVertices;
        std::set<size_t> visitedInFace;
        size_t v0 = edge.first;
        size_t v1 = edge.second;

        faceVertices.push_back(v0);
        visitedInFace.insert(v0);
        faceVertices.push_back(v1);
        visitedInFace.insert(v1);
        usedEdges.insert(edge);
        usedEdges.insert(reversed);

        bool faceClosed = false;
        while (!faceClosed) {
            size_t nextVertex = FindNextVertex(v1, v0);
            if (nextVertex == SIZE_MAX) break;

            if (nextVertex == faceVertices[0]) {
                faceClosed = true;
                break;
            }

            if (visitedInFace.count(nextVertex) > 0) break;

            visitedInFace.insert(nextVertex);
            faceVertices.push_back(nextVertex);
            usedEdges.insert({v1, nextVertex});
            usedEdges.insert({nextVertex, v1});

            v0 = v1;
            v1 = nextVertex;
        }

        if (faceClosed && faceVertices.size() >= 3) {
            Face newFace;
            newFace.vertexIndices = faceVertices;
            newFace.CalculateNormal(verts);

            bool isDuplicate = false;
            for (const auto& existingFace : faces) {
                if (existingFace.vertexIndices.size() == faceVertices.size()) {
                    std::set<size_t> existingSet(existingFace.vertexIndices.begin(),
                                                existingFace.vertexIndices.end());
                    std::set<size_t> newSet(faceVertices.begin(), faceVertices.end());
                    if (existingSet == newSet) { isDuplicate = true; break; }
                }
            }

            if (!isDuplicate) faces.push_back(newFace);
        }
    }
}

size_t Polyhedron3D::FindNextVertex(size_t fromVertex, size_t notThisVertex) {
    for (const auto& edge : communications) {
        if (edge.first == fromVertex && edge.second != notThisVertex) return edge.second;
        if (edge.second == fromVertex && edge.first != notThisVertex) return edge.first;
    }
    return SIZE_MAX;
}

void Polyhedron3D::CalculateTetrahedrons() {
    // TODO
}

Vector3f Polyhedron3D::ComputeLocalFaceNormal(const Face& face) const {
    if (face.vertexIndices.size() < 3) return Vector3f::Zero();
    const auto& verts = localGeometry.vertices;
    Vector3f p0 = verts[face.vertexIndices[0]].localPoint;
    for (size_t i = 1; i + 1 < face.vertexIndices.size(); i++) {
        Vector3f p1 = verts[face.vertexIndices[i]].localPoint;
        Vector3f p2 = verts[face.vertexIndices[i + 1]].localPoint;
        Vector3f n = (p1 - p0).Cross(p2 - p0);
        float len = n.Magnitude();
        if (len > 1e-6f) return n / len;
    }
    return Vector3f::Zero();
}

Vector3f Polyhedron3D::ComputeGlobalFaceNormal(const Face& face) const {
    if (face.vertexIndices.size() < 3) return Vector3f::Zero();
    const auto& verts = localGeometry.vertices;
    Vector3f p0 = verts[face.vertexIndices[0]].GetGlobalPoint(transform);
    for (size_t i = 1; i + 1 < face.vertexIndices.size(); i++) {
        Vector3f p1 = verts[face.vertexIndices[i]].GetGlobalPoint(transform);
        Vector3f p2 = verts[face.vertexIndices[i + 1]].GetGlobalPoint(transform);
        Vector3f n = (p1 - p0).Cross(p2 - p0);
        float len = n.Magnitude();
        if (len > 1e-6f) return n / len;
    }
    return Vector3f::Zero();
}

void Polyhedron3D::RebuildLocalGeometry() {
    const auto& verts = localGeometry.vertices;
    const auto& faces = localGeometry.faces;

    localGeometry.faceNormals.clear();
    localGeometry.edgeDirections.clear();

    if (verts.empty()) {
        localGeometry.center = Vector3f::Zero();
        localGeometry.aabb = AABB_3D(Vector3f::Zero(), Vector3f::Zero());
        localGeometry.valid = true;
        return;
    }

    Vector3f c = Vector3f::Zero();
    for (const auto& v : verts) c += v.localPoint;
    localGeometry.center = c / static_cast<float>(verts.size());

    Vector3f mn = verts[0].localPoint;
    Vector3f mx = mn;
    for (const auto& v : verts) {
        const Vector3f& p = v.localPoint;
        mn.x = std::min(mn.x, p.x); mn.y = std::min(mn.y, p.y); mn.z = std::min(mn.z, p.z);
        mx.x = std::max(mx.x, p.x); mx.y = std::max(mx.y, p.y); mx.z = std::max(mx.z, p.z);
    }
    localGeometry.aabb = AABB_3D(mn, mx);

    localGeometry.faceNormals.resize(faces.size());
    for (size_t i = 0; i < faces.size(); i++) {
        localGeometry.faceNormals[i] = ComputeLocalFaceNormal(faces[i]);
    }

    localGeometry.edgeDirections.resize(communications.size());
    for (size_t i = 0; i < communications.size(); i++) {
        const auto& e = communications[i];
        Vector3f d = verts[e.second].localPoint - verts[e.first].localPoint;
        float len = d.Magnitude();
        localGeometry.edgeDirections[i] = (len > 1e-6f) ? d / len : Vector3f::Zero();
    }

    localGeometry.valid = true;
}

void Polyhedron3D::RebuildGlobalGeometry() const {
    const auto& localVerts = localGeometry.vertices;
    const auto& faces = localGeometry.faces;

    globalGeometry.vertices.clear();
    globalGeometry.faceNormals.clear();
    globalGeometry.edgeDirections.clear();

    if (localVerts.empty()) {
        globalGeometry.center = Vector3f::Zero();
        globalGeometry.aabb = AABB_3D(Vector3f::Zero(), Vector3f::Zero());
        globalGeometry.valid = true;
        return;
    }

    globalGeometry.vertices.reserve(localVerts.size());
    for (const auto& v : localVerts) {
        globalGeometry.vertices.push_back(Vertex3D(v.GetGlobalPoint(transform)));
    }

    auto worldPoint = [&](size_t i) -> const Vector3f& {
        return globalGeometry.vertices[i].localPoint;
    };

    Vector3f c = Vector3f::Zero();
    for (size_t i = 0; i < globalGeometry.vertices.size(); i++) {
        c += worldPoint(i);
    }
    globalGeometry.center = c / static_cast<float>(globalGeometry.vertices.size());

    Vector3f mn = worldPoint(0);
    Vector3f mx = mn;
    for (size_t i = 1; i < globalGeometry.vertices.size(); i++) {
        const Vector3f& p = worldPoint(i);
        mn.x = std::min(mn.x, p.x); mn.y = std::min(mn.y, p.y); mn.z = std::min(mn.z, p.z);
        mx.x = std::max(mx.x, p.x); mx.y = std::max(mx.y, p.y); mx.z = std::max(mx.z, p.z);
    }
    globalGeometry.aabb = AABB_3D(mn, mx);

    globalGeometry.faceNormals.resize(faces.size());
    for (size_t i = 0; i < faces.size(); i++) {
        globalGeometry.faceNormals[i] = ComputeGlobalFaceNormal(faces[i]);
    }

    globalGeometry.edgeDirections.resize(communications.size());
    for (size_t i = 0; i < communications.size(); i++) {
        const auto& e = communications[i];
        Vector3f d = worldPoint(e.second) - worldPoint(e.first);
        float len = d.Magnitude();
        globalGeometry.edgeDirections[i] = (len > 1e-6f) ? d / len : Vector3f::Zero();
    }

    globalGeometry.valid = true;
}

void Polyhedron3D::EnsureGlobalGeometry() const {
    if (!globalGeometry.valid || transform->IsDirty()) {
        RebuildGlobalGeometry();
    }
}

const PolyhedronGeometry& Polyhedron3D::GetGlobalGeometry() const {
    EnsureGlobalGeometry();
    return globalGeometry;
}

Vector3f Polyhedron3D::GetCenter() const {
    return localGeometry.center;
}

Vector3f Polyhedron3D::GetGlobalCenter() const {
    EnsureGlobalGeometry();
    return globalGeometry.center;
}

const AABB_3D& Polyhedron3D::GetGlobalAABB() const {
    EnsureGlobalGeometry();
    return globalGeometry.aabb;
}

Vector3f Polyhedron3D::GetGlobalEdgeDirection(const std::pair<size_t, size_t>& edge) const {
    EnsureGlobalGeometry();
    const auto& verts = globalGeometry.vertices;
    Vector3f p1 = verts[edge.first].localPoint;
    Vector3f p2 = verts[edge.second].localPoint;
    Vector3f d = p2 - p1;
    float len = d.Magnitude();
    return (len > 1e-6f) ? d / len : Vector3f::Zero();
}

Vector3f Polyhedron3D::GetEdgeDirection(const std::pair<size_t, size_t>& edge) const {
    const auto& verts = localGeometry.vertices;
    Vector3f p1 = verts[edge.first].localPoint;
    Vector3f p2 = verts[edge.second].localPoint;
    return (p2 - p1).Normalized();
}

float Polyhedron3D::GetVolume() const {
    return 1.0f;
}

void Polyhedron3D::AddVertex(const Vector3f& newVertex) {
    localGeometry.vertices.push_back(Vertex3D(newVertex));
    ApplyChanged();
}

void Polyhedron3D::AddVertex(const Vector3f& newVertex, std::vector<size_t> vertexCommunications) {
    localGeometry.vertices.push_back(Vertex3D(newVertex));
    size_t myIndex = localGeometry.vertices.size() - 1;

    for (auto comm : vertexCommunications) {
        communications.push_back({myIndex, comm});
    }
    ApplyChanged();
}

void Polyhedron3D::AddCommunication(size_t fromIndex, size_t toIndex) {
    communications.push_back({fromIndex, toIndex});
    ApplyChanged();
}