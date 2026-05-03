#include "Polyhedron3D.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <set>
#include "GameObject.h"
#include "ColliderManager.h"

Polyhedron3D::Polyhedron3D(std::vector<Vector3> newVertices) {
    for(const auto& v : newVertices) {
        vertices.push_back(Vertex3D(v));
    }
    isChanged = true;
}

Polyhedron3D::~Polyhedron3D() {

}

void Polyhedron3D::ApplyChanged(){
    CalculateCenter();
    CalculateAABB();
    CalculateFaces();
    CalculateTetrahedrons();
}

Vector3 Polyhedron3D::GetEdgeDirection(const std::pair<size_t, size_t>& edge) const {
    Vector3 p1 = vertices[edge.first].localPoint;
    Vector3 p2 = vertices[edge.second].localPoint;
    return (p2 - p1).normalized();
}

void Polyhedron3D::CalculateTetrahedrons(){

}

void Polyhedron3D::CalculateFaces() {
    std::set<std::pair<size_t, size_t>> usedEdges;
    faces.clear();
    
    for (const auto& startEdge : communications) {
        auto edge = startEdge;
        auto reversed = std::make_pair(edge.second, edge.first);
        
        if (usedEdges.count(edge) > 0) continue;
        
        std::vector<size_t> faceVertices;
        size_t v0 = edge.first;
        size_t v1 = edge.second;
        
        faceVertices.push_back(v0);
        faceVertices.push_back(v1);
        usedEdges.insert(edge);
        usedEdges.insert(reversed);
        
        bool faceClosed = false;
        while (!faceClosed) {
            size_t nextVertex = FindNextVertex(v1, v0);
            if (nextVertex == SIZE_MAX) break;
            
            if (nextVertex == faceVertices[0]) {
                faceVertices.push_back(nextVertex);
                faceClosed = true;
                break;
            }
            
            faceVertices.push_back(nextVertex);
            usedEdges.insert({v1, nextVertex});
            usedEdges.insert({nextVertex, v1});
            
            v0 = v1;
            v1 = nextVertex;
        }
        
        if (faceClosed && faceVertices.size() >= 3) {
            Face newFace;
            newFace.vertexIndices = faceVertices;
            newFace.CalculateNormal(vertices);
            
            bool isDuplicate = false;
            for (const auto& existingFace : faces) {
                if (existingFace.vertexIndices.size() == faceVertices.size()) {
                    std::set<size_t> existingSet(existingFace.vertexIndices.begin(), 
                                                  existingFace.vertexIndices.end());
                    std::set<size_t> newSet(faceVertices.begin(), faceVertices.end());
                    if (existingSet == newSet) {
                        isDuplicate = true;
                        break;
                    }
                }
            }
            
            if (!isDuplicate) {
                faces.push_back(newFace);
            }
        }
    }
}

size_t Polyhedron3D::FindNextVertex(size_t fromVertex, size_t notThisVertex) {
    for (const auto& edge : communications) {
        if (edge.first == fromVertex && edge.second != notThisVertex) {
            return edge.second;
        }
        if (edge.second == fromVertex && edge.first != notThisVertex) {
            return edge.first;
        }
    }
    return 0;
}

void Polyhedron3D::CalculateCenter(){
    center = Vector3::Zero;
    for(const auto& vertex : vertices){
        center += vertex.GetGlobalPoint(*transform);
    }
    center /= vertices.size();
}

void Polyhedron3D::CalculateAABB(){
    if (vertices.empty()){
        cachedAABB = AABB(Vector3(0,0,0), Vector3(0,0,0));
        return;
    }

    Vector3 min = vertices[0].GetGlobalPoint(*transform);
    Vector3 max = min;
    
    for (const auto& vertex : vertices){
        Vector3 global = vertex.GetGlobalPoint(*transform);
        
        min.x = std::min(min.x, global.x);
        min.y = std::min(min.y, global.y);
        min.z = std::min(min.z, global.z);
        
        max.x = std::max(max.x, global.x);
        max.y = std::max(max.y, global.y);
        max.z = std::max(max.z, global.z);
    }
    
    cachedAABB = AABB(min, max);
}

bool Polyhedron3D::IntersectAABB(const Polyhedron3D* other) const{
    const AABB& otherAABB = other->GetAABB();

    return (cachedAABB.min.x <= otherAABB.max.x && cachedAABB.max.x >= otherAABB.min.x) &&
           (cachedAABB.min.y <= otherAABB.max.y && cachedAABB.max.y >= otherAABB.min.y) &&
           (cachedAABB.min.z <= otherAABB.max.z && cachedAABB.max.z >= otherAABB.min.z);
}

bool Polyhedron3D::IntersectSAT(const Polyhedron3D* other) const{
    std::vector<Vector3> allAxes;
    
    for(const auto& face : faces) {
        allAxes.push_back(face.normal);
    }
    
    for(const auto& face : other->faces) {
        allAxes.push_back(face.normal);
    }

    float minOverlap = std::numeric_limits<float>::max();
    Vector3 penetrationAxis = Vector3::Zero;
    for(const auto& myEdge : communications) {
        Vector3 myEdgeDir = GetEdgeDirection(myEdge);
        
        for(const auto& otherEdge : other->communications) {
            Vector3 otherEdgeDir = other->GetEdgeDirection(otherEdge);
            
            Vector3 axis = myEdgeDir.cross(otherEdgeDir);
            
            if(axis.magnitude() > 0.0001f) {
                axis.normalize();
                allAxes.push_back(axis);
            }
        }
    }

    for(const auto& axis : allAxes) {
        
        float myMin = 999999.0f;
        float myMax = -999999.0f;
        
        for(const auto& vertex : vertices) {
            Vector3 global = vertex.GetGlobalPoint(*transform);
            float proj = global.dot(axis);
            myMin = std::min(myMin, proj);
            myMax = std::max(myMax, proj);
        }
        
        float otherMin = 999999.0f;
        float otherMax = -999999.0f;
        
        for(const auto& vertex : other->vertices) {
            Vector3 global = vertex.GetGlobalPoint(*other->transform);
            float proj = global.dot(axis);
            otherMin = std::min(otherMin, proj);
            otherMax = std::max(otherMax, proj);
        }
        
        if(myMax < otherMin || otherMax < myMin){
            return false;
        }

        float overlap = std::min(myMax, otherMax) - std::max(myMin, otherMin);
        
        Vector3 direction = axis;
        
        if(myMax > otherMax){
            direction = -direction;
        }
        
        if(overlap < minOverlap){
            minOverlap = overlap;
            penetrationAxis = direction * overlap;
        }
    }

    const_cast<Polyhedron3D*>(this)->AddOverlap(
        const_cast<BaseCollider*>(static_cast<const BaseCollider*>(other)), 
        penetrationAxis
    );
    
    return true;
}

bool Polyhedron3D::CheckExactIntersection(const Polyhedron3D* other) const{
    return true;
}

void Polyhedron3D::Start(){
    transform = gameObject->GetComponentOfType<Transform3D>();
    isChanged = true;
}

void Polyhedron3D::Update(){
    if (vertices.empty() || !transform) return;

    if(isChanged){
        ApplyChanged();
    }
}

bool Polyhedron3D::ContainsPoint(const Vector3& point) const {
    if (point.x < cachedAABB.min.x || point.x > cachedAABB.max.x ||
        point.y < cachedAABB.min.y || point.y > cachedAABB.max.y ||
        point.z < cachedAABB.min.z || point.z > cachedAABB.max.z) {
        return false;
    }
    
    for (const auto& tetra : tetrahedrons) {
        if (tetra.ContainsPoint(point, *transform)) {
            return true;
        }
    }
    
    return false;
}

bool Polyhedron3D::Intersects(const BaseCollider* other) const {
    const Polyhedron3D* otherPoly = dynamic_cast<const Polyhedron3D*>(other);
    
    if (!otherPoly) return false;
    
    if(!IntersectAABB(otherPoly)) return false;
    if(!IntersectSAT(otherPoly)) return false;

    return CheckExactIntersection(otherPoly);
}

Vector3 Polyhedron3D::GetCenter() const{
    return center;
}

float Polyhedron3D::GetVolume() const{
    return 1.0f;
}

void Polyhedron3D::AddVertex(const Vector3& newVertex){
    isChanged = true;
}
void Polyhedron3D::AddVertex(const Vector3& newVertex, std::vector<size_t> vertexCommunications){
    isChanged = true;
}
void Polyhedron3D::AddCommunication(size_t fromIndex, size_t toIndex){
    isChanged = true;
}

std::vector<Segment3D> Polyhedron3D::GetIntersectionSegments(const Polyhedron3D& other) const {
    return std::vector<Segment3D>();
}