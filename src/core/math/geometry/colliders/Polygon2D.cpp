#include "include/core/math/geometry/colliders/Polygon2D.h"
#include <algorithm>
#include <limits>
#include <cmath> 
#include "include/core/GameObject.h"
#include "include/core/math/geometry/colliders_manager/ColliderManager.h" 

Polygon2D::Polygon2D(const std::vector<Vector2>& vertices) 
    : localVertices(vertices), worldVerticesValid(false), centerValid(false) {}

Polygon2D::Polygon2D(std::vector<Vector2>&& vertices) 
    : localVertices(std::move(vertices)), worldVerticesValid(false), centerValid(false) {}

Polygon2D Polygon2D::FromSegments(const std::vector<Segment>& segments) {
    std::vector<Vector2> vertices;
    if (!segments.empty()) {
        vertices.push_back(segments[0].point1);
        
        for (const auto& seg : segments) {
            vertices.push_back(seg.point2);
        }
        
        if (vertices.size() > 1 && vertices.front() == vertices.back()) {
            vertices.pop_back();
        }
    }
    return Polygon2D(std::move(vertices));
}

void Polygon2D::Start() {
    transform = GetTransformComponent();
}

void Polygon2D::Update() {
    if (transform) {
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon2D::validateWorldVertices() const {
    if (!worldVerticesValid && transform && !localVertices.empty()) {
        worldVerticesCache.clear();
        worldVerticesCache.reserve(localVertices.size());
        
        for (const auto& v : localVertices) {
            worldVerticesCache.push_back(applyTransformToPoint(v, transform));
        }
        
        worldVerticesValid = true;
    }
}

const std::vector<Vector2>& Polygon2D::getWorldVertices() const {
    validateWorldVertices();
    return worldVerticesValid ? worldVerticesCache : localVertices;
}

std::vector<Segment> Polygon2D::buildSegmentsFromVertices(const std::vector<Vector2>& vertices) const {
    std::vector<Segment> segments;
    
    if (vertices.size() < 2) {
        return segments;
    }
    
    segments.reserve(vertices.size());
    
    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        segments.emplace_back(vertices[i], vertices[i + 1]);
    }
    
    if (vertices.size() > 2) {
        segments.emplace_back(vertices.back(), vertices.front());
    }
    
    return segments;
}

std::vector<Segment> Polygon2D::getWorldSegments() const {
    return buildSegmentsFromVertices(getWorldVertices());
}

std::vector<Segment> Polygon2D::getLocalSegments() const {
    return buildSegmentsFromVertices(localVertices);
}

Vector2 Polygon2D::applyTransformToPoint(const Vector2& point, const Transform2D* t) const {
    if (!t) return point;
    
    Vector2 result = point;
    
    result.x *= t->scale.x;
    result.y *= t->scale.y;
    
    float angleRad = t->rotation * 3.1415f / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    
    float x = result.x * cosA - result.y * sinA;
    float y = result.x * sinA + result.y * cosA;
    result.x = x;
    result.y = y;
    
    result.x += t->position.x;
    result.y += t->position.y;
    
    return result;
}

const Transform2D* Polygon2D::GetTransformComponent() const {
    if (!gameObject) return nullptr;
    return gameObject->GetComponentOfType<Transform2D>();
}

void Polygon2D::setVertices(const std::vector<Vector2>& vertices) {
    localVertices = vertices;
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon2D::setVertices(std::vector<Vector2>&& vertices) {
    localVertices = std::move(vertices);
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon2D::addVertex(const Vector2& vertex) {
    localVertices.push_back(vertex);
    worldVerticesValid = false;
    centerValid = false;
}

void Polygon2D::insertVertex(size_t index, const Vector2& vertex) {
    if (index <= localVertices.size()) {
        localVertices.insert(localVertices.begin() + index, vertex);
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon2D::removeVertex(size_t index) {
    if (index < localVertices.size()) {
        localVertices.erase(localVertices.begin() + index);
        worldVerticesValid = false;
        centerValid = false;
    }
}

void Polygon2D::clear() {
    localVertices.clear();
    worldVerticesCache.clear();
    worldVerticesValid = false;
    centerValid = false;
}

bool Polygon2D::containsPoint(const Vector2& point) const {
    const auto& vertices = getWorldVertices();
    if (vertices.size() < 3) return false;
    
    float maxX = std::numeric_limits<float>::lowest();
    for (const auto& v : vertices) {
        maxX = std::max(maxX, v.x);
    }
    Vector2 rayEnd(maxX + 1.0f, point.y);
    Segment ray(point, rayEnd);
    
    int intersectionCount = 0;
    auto segments = getWorldSegments();
    
    for (const auto& seg : segments) {
        if (IntersectionSegments2D::checkIntersection(ray, seg)) {
            intersectionCount++;
        }
    }
    
    return (intersectionCount % 2) == 1;
}

bool Polygon2D::intersects(const Polygon2D& other) const {
    if (!intersectsFast(other)) {
        return false;
    }
    
    auto segs1 = getWorldSegments();
    auto segs2 = other.getWorldSegments();
    
    for (const auto& seg1 : segs1) {
        for (const auto& seg2 : segs2) {
            if (IntersectionSegments2D::checkIntersection(seg1, seg2)) {
                return true;
            }
        }
    }
    
    if (containsPoint(other.getCenter()) || other.containsPoint(getCenter())) {
        return true;
    }
    
    return false;
}

bool Polygon2D::intersectsFast(const Polygon2D& other) const {
    auto verts1 = getWorldVertices();
    auto verts2 = other.getWorldVertices();
    
    if (verts1.empty() || verts2.empty()) {
        return false;
    }
    
    float minX1 = std::numeric_limits<float>::max();
    float maxX1 = std::numeric_limits<float>::lowest();
    float minY1 = std::numeric_limits<float>::max();
    float maxY1 = std::numeric_limits<float>::lowest();
    
    for (const auto& v : verts1) {
        minX1 = std::min(minX1, v.x);
        maxX1 = std::max(maxX1, v.x);
        minY1 = std::min(minY1, v.y);
        maxY1 = std::max(maxY1, v.y);
    }
    
    float minX2 = std::numeric_limits<float>::max();
    float maxX2 = std::numeric_limits<float>::lowest();
    float minY2 = std::numeric_limits<float>::max();
    float maxY2 = std::numeric_limits<float>::lowest();
    
    for (const auto& v : verts2) {
        minX2 = std::min(minX2, v.x);
        maxX2 = std::max(maxX2, v.x);
        minY2 = std::min(minY2, v.y);
        maxY2 = std::max(maxY2, v.y);
    }
    
    return !(maxX1 < minX2 || minX1 > maxX2 ||
             maxY1 < minY2 || minY1 > maxY2);
}

void Polygon2D::calculateCenter() const {
    if (localVertices.empty()) {
        center = Vector2(0, 0);
        centerValid = true;
        return;
    }
    
    const auto& verts = getWorldVertices();
    Vector2 sum(0, 0);
    
    for (const auto& v : verts) {
        sum += v;
    }
    
    center = sum / static_cast<float>(verts.size());
    centerValid = true;
}

const Vector2& Polygon2D::getCenter() const {
    if (!centerValid) {
        calculateCenter();
    }
    return center;
}