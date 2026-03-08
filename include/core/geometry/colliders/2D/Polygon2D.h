#pragma once

#include <vector>
#include <string>
#include <memory>
#include "InserectionSegments2D.h"
#include "Collider.h"
#include "BaseCollider.h"
#include "Transform2D.h"
#include "Vector2.h"

class ColliderManager;

class Polygon2D : public Collider<Transform2D, Vector2> {
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
    const Transform2D* GetTransform2D() const;

public:
    Polygon2D() = default;
    explicit Polygon2D(const std::vector<Vector2>& vertices);
    explicit Polygon2D(std::vector<Vector2>&& vertices);
    
    static Polygon2D FromSegments(const std::vector<Segment>& segments);
    
    void Start() override;
    void Update() override;

    float GetVolume() const override;
    
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
    
    bool ContainsPoint(const Vector2& point) const override;
    bool Intersects(const BaseCollider* other) const override;

    Vector2 GetCenter() const override;    
    
    bool containsPoint(const Vector2& point) const;
    bool intersectsFast(const Polygon2D& other) const;
    
    const Vector2& getCenter() const;
    size_t vertexCount() const { return localVertices.size(); }
    bool isEmpty() const { return localVertices.empty(); }
    
    Polygon2D(const Polygon2D& other) = default;
    Polygon2D(Polygon2D&& other) noexcept = default;
    Polygon2D& operator=(const Polygon2D& other) = default;
    Polygon2D& operator=(Polygon2D&& other) noexcept = default;
    virtual ~Polygon2D() override = default; 

    bool GetCollisionInfo(const BaseCollider* other, 
                          Vector2& point, 
                          Vector2& normal, 
                          float& penetration) const override;
};