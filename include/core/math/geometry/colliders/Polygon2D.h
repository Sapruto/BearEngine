#pragma once

#include <vector>
#include <string>
#include <memory>
#include "include/core/math/geometry/InserectionSegments2D.h"
#include "include/core/math/Transform2D.h"
#include "include/core/math/geometry/colliders/Collider.h"

class ColliderManager;

class Polygon2D : public Collider {
private:
    std::vector<Vector2> localVertices;
    mutable std::vector<Vector2> worldVerticesCache;
    mutable bool worldVerticesValid{false};
    
    mutable Vector2 center;
    mutable bool centerValid{false};
    
    void validateWorldVertices() const;
    void calculateCenter() const;
    
    std::vector<Segment> buildSegmentsFromVertices(const std::vector<Vector2>& vertices) const;
    
    Vector2 applyTransformToPoint(const Vector2& point, const Transform2D* transform) const;
    const Transform2D* GetTransformComponent() const;

public:
    Polygon2D() = default;
    explicit Polygon2D(const std::vector<Vector2>& vertices);
    explicit Polygon2D(std::vector<Vector2>&& vertices);
    
    static Polygon2D FromSegments(const std::vector<Segment>& segments);
    
    void Start() override;
    void Update() override;
    
    void setVertices(const std::vector<Vector2>& vertices);
    void setVertices(std::vector<Vector2>&& vertices);
    const std::vector<Vector2>& getLocalVertices() const { return localVertices; }
    const std::vector<Vector2>& getWorldVertices() const;
    
    std::vector<Segment> getWorldSegments() const;
    std::vector<Segment> getLocalSegments() const;
    
    void addVertex(const Vector2& vertex);
    void insertVertex(size_t index, const Vector2& vertex);
    void removeVertex(size_t index);
    void clear();
    
    bool containsPoint(const Vector2& point) const;
    bool intersects(const Polygon2D& other) const;
    bool intersectsFast(const Polygon2D& other) const;
    
    const Vector2& getCenter() const;
    size_t vertexCount() const { return localVertices.size(); }
    bool isEmpty() const { return localVertices.empty(); }
    
    bool ContainsPoint(const Vector2& point) const override { return containsPoint(point); }
    bool Intersects(const Collider* other) const override { 
        const Polygon2D* otherPoly = dynamic_cast<const Polygon2D*>(other);
        if (otherPoly) {
            return intersects(*otherPoly);
        }
        return false;
    }
    Vector2 GetCenter() const override { return getCenter(); }
    
    Polygon2D(const Polygon2D& other) = default;
    Polygon2D(Polygon2D&& other) noexcept = default;
    Polygon2D& operator=(const Polygon2D& other) = default;
    Polygon2D& operator=(Polygon2D&& other) noexcept = default;
    virtual ~Polygon2D() override = default; 

    bool GetCollisionInfo(const Collider* other, 
                          Vector2& point, 
                          Vector2& normal, 
                          float& penetration) const override {
        return false;
    }
};