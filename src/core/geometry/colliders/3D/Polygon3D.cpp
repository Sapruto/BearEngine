#include "Polygon3D.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include "GameObject.h"
#include "ColliderManager.h"

Polygon3D::Polygon3D(const std::vector<Vector3>& vertices)
    : localVertices(vertices), worldVerticesValid(false), centerValid(false) {
    if (vertices.size() >= 3) {
        for (size_t i = 0; i < vertices.size(); ++i) {
            edges.emplace_back(i, (i + 1) % vertices.size());
        }
    }
}

Polygon3D::Polygon3D(std::vector<Vector3>&& vertices)
    : localVertices(std::move(vertices)), worldVerticesValid(false), centerValid(false) {
    if (localVertices.size() >= 3) {
        for (size_t i = 0; i < localVertices.size(); ++i) {
            edges.emplace_back(i, (i + 1) % localVertices.size());
        }
    }
}

Polygon3D::Polygon3D(const std::vector<Vector3>& vertices, const std::vector<Edge>& edges)
    : localVertices(vertices), edges(edges), worldVerticesValid(false), centerValid(false) {
}

Polygon3D::Polygon3D(std::vector<Vector3>&& vertices, std::vector<Edge>&& edges)
    : localVertices(std::move(vertices)), edges(std::move(edges)), 
      worldVerticesValid(false), centerValid(false) {
}

Polygon3D Polygon3D::FromSegments(const std::vector<Segment3D>& segments) {
    std::vector<Vector3> vertices;
    std::vector<Edge> edges;
    
    if (segments.empty()) {
        return Polygon3D();
    }
    
    vertices.push_back(segments[0].point1);
    
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& seg = segments[i];
        
        auto it = std::find_if(vertices.begin(), vertices.end(),
            [&seg](const Vector3& v) { return v == seg.point2; });
            
        if (it == vertices.end()) {
            vertices.push_back(seg.point2);
        }
        
        size_t idx1 = std::distance(vertices.begin(),
            std::find(vertices.begin(), vertices.end(), seg.point1));
        size_t idx2 = std::distance(vertices.begin(),
            std::find(vertices.begin(), vertices.end(), seg.point2));
            
        edges.emplace_back(idx1, idx2);
    }
    
    return Polygon3D(std::move(vertices), std::move(edges));
}

Polygon3D Polygon3D::CreateRectangle(float width, float height, float depth) {
    std::vector<Vector3> vertices;
    std::vector<Edge> edges;
    
    vertices.push_back(Vector3(-width/2, -height/2, -depth/2));
    vertices.push_back(Vector3( width/2, -height/2, -depth/2));
    vertices.push_back(Vector3( width/2,  height/2, -depth/2));
    vertices.push_back(Vector3(-width/2,  height/2, -depth/2));
    
    for (size_t i = 0; i < 4; ++i) {
        edges.emplace_back(i, (i + 1) % 4);
    }
    
    if (std::abs(depth) > 1e-6f) {
        size_t count = vertices.size();
        for (size_t i = 0; i < count; ++i) {
            Vector3 v = vertices[i];
            vertices.push_back(Vector3(v.x, v.y, v.z + depth));
        }
        
        for (size_t i = 4; i < 8; ++i) {
            edges.emplace_back(i, (i == 7) ? 4 : i + 1);
        }
        
        for (size_t i = 0; i < 4; ++i) {
            edges.emplace_back(i, i + 4);
        }
    }
    
    return Polygon3D(std::move(vertices), std::move(edges));
}

Polygon3D Polygon3D::CreateConvexHull(const std::vector<Vector3>& points) {
    if (points.size() < 3) return Polygon3D();
    
    std::vector<Vector3> vertices = points;
    std::sort(vertices.begin(), vertices.end(),
        [](const Vector3& a, const Vector3& b) {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        });
    
    std::vector<Vector3> hull;
    
    for (const auto& p : vertices) {
        while (hull.size() >= 2) {
            Vector3 a = hull[hull.size() - 2];
            Vector3 b = hull[hull.size() - 1];
            Vector3 c = p;
            
            Vector3 ab = b - a;
            Vector3 ac = c - a;
            Vector3 cross = ab.cross(ac);
            
            if (cross.z <= 0) break;
            hull.pop_back();
        }
        hull.push_back(p);
    }
    
    size_t lowerSize = hull.size();
    for (int i = static_cast<int>(vertices.size()) - 2; i >= 0; --i) {
        const auto& p = vertices[i];
        while (hull.size() > lowerSize) {
            Vector3 a = hull[hull.size() - 2];
            Vector3 b = hull[hull.size() - 1];
            Vector3 c = p;
            
            Vector3 ab = b - a;
            Vector3 ac = c - a;
            Vector3 cross = ab.cross(ac);
            
            if (cross.z <= 0) break;
            hull.pop_back();
        }
        hull.push_back(p);
    }
    
    hull.pop_back();
    
    return Polygon3D(std::move(hull));
}

const Transform3D* Polygon3D::GetTransform3D() const {
    return transform;
}

void Polygon3D::Start() {
    if (gameObject) {
        transform = gameObject->GetComponentOfType<Transform3D>();
    }
}

void Polygon3D::Update() {
    if (transform) {
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon3D::validateWorldVertices() const {
    const Transform3D* t3d = GetTransform3D();
    if (!worldVerticesValid && t3d && !localVertices.empty()) {
        worldVerticesCache.clear();
        worldVerticesCache.reserve(localVertices.size());
        
        for (const auto& v : localVertices) {
            worldVerticesCache.push_back(applyTransformToPoint(v, t3d));
        }
        
        worldVerticesValid = true;
    }
}

const std::vector<Vector3>& Polygon3D::getWorldVertices() const {
    validateWorldVertices();
    return worldVerticesValid ? worldVerticesCache : localVertices;
}

Vector3 Polygon3D::applyTransformToPoint(const Vector3& point, const Transform3D* t) const {
    if (!t) return point;
    
    Vector3 result = point;
    
    result.x *= t->scale.x;
    result.y *= t->scale.y;
    result.z *= t->scale.z;
    
    float angleRad = t->rotation.z * 3.14159265359f / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    
    float x = result.x * cosA - result.y * sinA;
    float y = result.x * sinA + result.y * cosA;
    float z = result.z;
    
    result.x = x + t->position.x;
    result.y = y + t->position.y;
    result.z = z + t->position.z;
    
    return result;
}

std::vector<Segment3D> Polygon3D::buildSegmentsFromEdges(const std::vector<Vector3>& vertices, const std::vector<Edge>& edges) const {
    std::vector<Segment3D> segments;
    segments.reserve(edges.size());
    
    for (const auto& edge : edges) {
        if (edge.vertexIndex1 < vertices.size() && edge.vertexIndex2 < vertices.size()) {
            segments.emplace_back(vertices[edge.vertexIndex1], vertices[edge.vertexIndex2]);
        }
    }
    
    return segments;
}

std::vector<Segment3D> Polygon3D::getWorldSegments() const {
    return buildSegmentsFromEdges(getWorldVertices(), edges);
}

std::vector<Segment3D> Polygon3D::getLocalSegments() const {
    return buildSegmentsFromEdges(localVertices, edges);
}

void Polygon3D::setVertices(const std::vector<Vector3>& vertices) {
    localVertices = vertices;
    edges.clear();
    if (vertices.size() >= 3) {
        for (size_t i = 0; i < vertices.size(); ++i) {
            edges.emplace_back(i, (i + 1) % vertices.size());
        }
    }
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon3D::setVertices(std::vector<Vector3>&& vertices) {
    localVertices = std::move(vertices);
    edges.clear();
    if (localVertices.size() >= 3) {
        for (size_t i = 0; i < localVertices.size(); ++i) {
            edges.emplace_back(i, (i + 1) % localVertices.size());
        }
    }
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon3D::addVertex(const Vector3& vertex) {
    localVertices.push_back(vertex);
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon3D::insertVertex(size_t index, const Vector3& vertex) {
    if (index <= localVertices.size()) {
        localVertices.insert(localVertices.begin() + index, vertex);
        
        for (auto& edge : edges) {
            if (edge.vertexIndex1 >= index) edge.vertexIndex1++;
            if (edge.vertexIndex2 >= index) edge.vertexIndex2++;
        }
        
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon3D::removeVertex(size_t index) {
    if (index < localVertices.size()) {
        removeEdgesWithVertex(index);
        
        localVertices.erase(localVertices.begin() + index);
        
        for (auto& edge : edges) {
            if (edge.vertexIndex1 > index) edge.vertexIndex1--;
            if (edge.vertexIndex2 > index) edge.vertexIndex2--;
        }
        
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon3D::setEdges(const std::vector<Edge>& newEdges) {
    edges = newEdges;
    worldVerticesValid = false;
}

void Polygon3D::setEdges(std::vector<Edge>&& newEdges) {
    edges = std::move(newEdges);
    worldVerticesValid = false;
}

void Polygon3D::addEdge(size_t v1, size_t v2) {
    if (v1 < localVertices.size() && v2 < localVertices.size()) {
        Edge newEdge(v1, v2);
        if (std::find(edges.begin(), edges.end(), newEdge) == edges.end()) {
            edges.push_back(newEdge);
            worldVerticesValid = false;
        }
    }
}

void Polygon3D::addEdge(const Edge& edge) {
    addEdge(edge.vertexIndex1, edge.vertexIndex2);
}

void Polygon3D::removeEdge(size_t v1, size_t v2) {
    Edge edge(v1, v2);
    auto it = std::find(edges.begin(), edges.end(), edge);
    if (it != edges.end()) {
        edges.erase(it);
        worldVerticesValid = false;
    }
}

void Polygon3D::removeEdge(const Edge& edge) {
    removeEdge(edge.vertexIndex1, edge.vertexIndex2);
}

void Polygon3D::removeEdgesWithVertex(size_t vertexIndex) {
    edges.erase(
        std::remove_if(edges.begin(), edges.end(),
            [vertexIndex](const Edge& e) {
                return e.vertexIndex1 == vertexIndex || e.vertexIndex2 == vertexIndex;
            }),
        edges.end()
    );
    worldVerticesValid = false;
}

bool Polygon3D::HasEdge(size_t v1, size_t v2) const {
    return std::find(edges.begin(), edges.end(), Edge(v1, v2)) != edges.end();
}

void Polygon3D::clear() {
    localVertices.clear();
    edges.clear();
    worldVerticesCache.clear();
    worldVerticesValid = false;
    centerValid = false;
}

bool Polygon3D::ContainsPoint(const Vector3& point) const {
    const auto& vertices = getWorldVertices();
    if (vertices.size() < 3) return false;
    
    float maxX = vertices[0].x;
    for (const auto& v : vertices) {
        if (v.x > maxX) maxX = v.x;
    }
    
    Vector3 rayEnd(maxX + 1.0f, point.y, point.z);
    Segment3D ray(point, rayEnd);
    
    int intersectionCount = 0;
    auto segments = getWorldSegments();
    
    for (const auto& seg : segments) {
        if (IntersectionSegments3D::CheckIntersection(ray, seg)) {
            intersectionCount++;
        }
    }
    
    return (intersectionCount % 2) == 1;
}

bool Polygon3D::Intersects(const BaseCollider* other) const {
    const Polygon3D* otherPoly = dynamic_cast<const Polygon3D*>(other);
    if (otherPoly) {
        return intersects(*otherPoly);
    }
    return false;
} 

Vector3 Polygon3D::GetCenter() const {
    return getCenter3D();
}

bool Polygon3D::GetCollisionInfo(const BaseCollider* other, Vector3& point, Vector3& normal, float& penetration) const {
    const Polygon3D* otherPoly = dynamic_cast<const Polygon3D*>(other);
    if (!otherPoly) return false;
    return false;
}

bool Polygon3D::intersects(const Polygon3D& other) const {
    if (!intersectsFast(other)) {
        return false;
    }
    
    auto segs1 = getWorldSegments();
    auto segs2 = other.getWorldSegments();
    
    for (const auto& seg1 : segs1) {
        for (const auto& seg2 : segs2) {
            if (IntersectionSegments3D::CheckIntersection(seg1, seg2)) {
                return true;
            }
        }
    }
    
    if (ContainsPoint(other.getCenter3D()) || other.ContainsPoint(getCenter3D())) {
        return true;
    }
    
    return false;
}

bool Polygon3D::intersectsFast(const Polygon3D& other) const {
    auto verts1 = getWorldVertices();
    auto verts2 = other.getWorldVertices();
    
    if (verts1.empty() || verts2.empty()) {
        return false;
    }
    
    float minX1 = verts1[0].x, maxX1 = verts1[0].x;
    float minY1 = verts1[0].y, maxY1 = verts1[0].y;
    float minZ1 = verts1[0].z, maxZ1 = verts1[0].z;
    
    for (const auto& v : verts1) {
        float x = v.x, y = v.y, z = v.z;
        if (x < minX1) minX1 = x;
        if (x > maxX1) maxX1 = x;
        if (y < minY1) minY1 = y;
        if (y > maxY1) maxY1 = y;
        if (z < minZ1) minZ1 = z;
        if (z > maxZ1) maxZ1 = z;
    }
    
    float minX2 = verts2[0].x, maxX2 = verts2[0].x;
    float minY2 = verts2[0].y, maxY2 = verts2[0].y;
    float minZ2 = verts2[0].z, maxZ2 = verts2[0].z;
    
    for (const auto& v : verts2) {
        float x = v.x, y = v.y, z = v.z;
        if (x < minX2) minX2 = x;
        if (x > maxX2) maxX2 = x;
        if (y < minY2) minY2 = y;
        if (y > maxY2) maxY2 = y;
        if (z < minZ2) minZ2 = z;
        if (z > maxZ2) maxZ2 = z;
    }
    
    return !(maxX1 < minX2 || minX1 > maxX2 ||
             maxY1 < minY2 || minY1 > maxY2 ||
             maxZ1 < minZ2 || minZ1 > maxZ2);
}

void Polygon3D::calculateCenter() const {
    if (localVertices.empty()) {
        center = Vector3(0, 0, 0);
        centerValid = true;
        return;
    }
    
    const auto& verts = getWorldVertices();
    float sumX = 0, sumY = 0, sumZ = 0;
    
    for (const auto& v : verts) {
        sumX += v.x;
        sumY += v.y;
        sumZ += v.z;
    }
    
    float count = static_cast<float>(verts.size());
    center = Vector3(sumX / count, sumY / count, sumZ / count);
    centerValid = true;
}

const Vector3& Polygon3D::getCenter3D() const {
    if (!centerValid) {
        calculateCenter();
    }
    return center;
}

bool Polygon3D::IsPolygonValid() const {
    if (localVertices.size() < 3) return false;
    if (edges.empty()) return false;
    
    std::vector<bool> vertexUsed(localVertices.size(), false);
    for (const auto& edge : edges) {
        if (edge.vertexIndex1 >= localVertices.size() || 
            edge.vertexIndex2 >= localVertices.size()) {
            return false;
        }
        vertexUsed[edge.vertexIndex1] = true;
        vertexUsed[edge.vertexIndex2] = true;
    }
    
    for (bool used : vertexUsed) {
        if (!used) return false;
    }
    
    auto components = GetConnectedComponents();
    return components.size() == 1;
}

std::vector<std::vector<size_t>> Polygon3D::GetConnectedComponents() const {
    std::vector<std::vector<size_t>> components;
    std::vector<bool> visited(localVertices.size(), false);
    
    for (size_t i = 0; i < localVertices.size(); ++i) {
        if (!visited[i]) {
            std::vector<size_t> component;
            std::vector<size_t> stack = {i};
            visited[i] = true;
            
            while (!stack.empty()) {
                size_t v = stack.back();
                stack.pop_back();
                component.push_back(v);
                
                for (const auto& edge : edges) {
                    if (edge.vertexIndex1 == v && !visited[edge.vertexIndex2]) {
                        visited[edge.vertexIndex2] = true;
                        stack.push_back(edge.vertexIndex2);
                    }
                    if (edge.vertexIndex2 == v && !visited[edge.vertexIndex1]) {
                        visited[edge.vertexIndex1] = true;
                        stack.push_back(edge.vertexIndex1);
                    }
                }
            }
            
            components.push_back(component);
        }
    }
    
    return components;
}

std::vector<Polygon3D> Polygon3D::SplitIntoPolygons() const {
    std::vector<Polygon3D> result;
    auto components = GetConnectedComponents();
    
    for (const auto& comp : components) {
        Polygon3D poly;
        std::vector<size_t> indexMap(localVertices.size(), static_cast<size_t>(-1));
        
        for (size_t i = 0; i < comp.size(); ++i) {
            poly.addVertex(localVertices[comp[i]]);
            indexMap[comp[i]] = i;
        }
        
        for (const auto& edge : edges) {
            if (indexMap[edge.vertexIndex1] != static_cast<size_t>(-1) && 
                indexMap[edge.vertexIndex2] != static_cast<size_t>(-1)) {
                poly.addEdge(indexMap[edge.vertexIndex1], indexMap[edge.vertexIndex2]);
            }
        }
        
        result.push_back(poly);
    }
    
    return result;
}

std::vector<Segment3D> Polygon3D::GetIntersectionSegments(const Polygon3D& other) const {
    std::vector<Segment3D> result;
    
    auto segs1 = getWorldSegments();
    auto segs2 = other.getWorldSegments();
    
    for (const auto& seg1 : segs1) {
        for (const auto& seg2 : segs2) {
            Vector3 intersectionPoint;
            if (IntersectionSegments3D::GetIntersectionPoint(seg1, seg2, intersectionPoint)) {
                result.emplace_back(intersectionPoint, intersectionPoint);
            }
        }
    }
    
    return result;
}

float Polygon3D::GetVolume() const {
    if (localVertices.size() < 4) return 0.0f;
    
    float minX = localVertices[0].x, maxX = localVertices[0].x;
    float minY = localVertices[0].y, maxY = localVertices[0].y;
    float minZ = localVertices[0].z, maxZ = localVertices[0].z;
    
    for (const auto& v : localVertices) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.z > maxZ) maxZ = v.z;
    }
    
    float width = maxX - minX;
    float height = maxY - minY;
    float depth = maxZ - minZ;
    
    return width * height * depth;
}