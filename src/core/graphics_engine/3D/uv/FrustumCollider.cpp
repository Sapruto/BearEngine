#include "FrustumCollider.h"

#include "Camera3D.h"
#include <cmath>

Vector3f FrustumCollider::UnprojectPoint(const Vector3f& ndc, const Matrix4x4f& invViewProj) {
    Vector3f world = invViewProj * ndc;
    return world;
}

void FrustumCollider::BuildFrustumPolyhedron(const Matrix4x4f& viewProj) {
    Matrix4x4f invViewProj = viewProj.inverse();
    
    std::vector<Vector3f> corners;
    corners.reserve(8);
    
    Vector3f ndcCorners[8] = {
        {-1, -1, -1}, {1, -1, -1}, {-1, 1, -1}, {1, 1, -1},
        {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1}, {1, 1, 1}
    };
    
    for (const auto& ndc : ndcCorners) {
        corners.push_back(UnprojectPoint(ndc, invViewProj));
    }
    
    auto& verts = GetVertices();
    verts.clear();
    
    for (const auto& corner : corners) {
        verts.emplace_back(corner);
    }
    
    auto& faceList = GetFaces();
    faceList.clear();
    
    faceList.emplace_back(std::vector<size_t>{0, 1, 3, 2});
    faceList.emplace_back(std::vector<size_t>{4, 6, 7, 5});
    faceList.emplace_back(std::vector<size_t>{0, 4, 5, 1});
    faceList.emplace_back(std::vector<size_t>{2, 3, 7, 6});
    faceList.emplace_back(std::vector<size_t>{0, 2, 6, 4});
    faceList.emplace_back(std::vector<size_t>{1, 5, 7, 3});
    
    for (auto& face : faceList) {
        face.CalculateNormal(verts);
    }
    
    communications.clear();
    
    for (size_t i = 0; i < verts.size(); i++) {
        for (size_t j = i + 1; j < verts.size(); j++) {
            bool connected = false;
            for (const auto& face : faceList) {
                bool hasI = false, hasJ = false;
                for (const auto& idx : face.vertexIndices) {
                    if (idx == i) hasI = true;
                    if (idx == j) hasJ = true;
                }
                if (hasI && hasJ) {
                    connected = true;
                    break;
                }
            }
            if (connected) {
                communications.emplace_back(i, j);
            }
        }
    }
    
    CalculateCenter();
    CalculateAABB();
    CalculateTetrahedrons();
    isChanged = false;
}

void FrustumCollider::UpdateFrustum() {
    if (!camera) return;
    
    Matrix4x4f view = camera->GetViewMatrix();
    Matrix4x4f proj = camera->GetProjectionMatrix(aspect);
    Matrix4x4f viewProj = proj * view;
    
    bool matricesEqual = true;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (std::abs(lastViewProj(i, j) - viewProj(i, j)) > 0.0001f) {
                matricesEqual = false;
                break;
            }
        }
        if (!matricesEqual) break;
    }
    
    if (!matricesEqual) {
        lastViewProj = viewProj;
        dirty = true;
    }
    
    if (dirty) {
        BuildFrustumPolyhedron(viewProj);
        dirty = false;
    }
}

void FrustumCollider::Update() {
    UpdateFrustum();
}

FrustumCollider::FrustumCollider(Camera3D* camera, float aspect, float fov, float near, float far) 
    : camera(camera), aspect(aspect), fov(fov), near(near), far(far) {
    lastViewProj = Matrix4x4f::Identity();
    dirty = true;
    UpdateFrustum();
}

bool FrustumCollider::IsVisible(const Polyhedron3D* object) const {
    if (!object) return false;
    return Intersects(object);
}

bool FrustumCollider::IsVisible(const AABB& aabb) const {
    if (vertices.empty()) return false;
    
    Vector3f corners[8] = {
        {aabb.min.x, aabb.min.y, aabb.min.z},
        {aabb.max.x, aabb.min.y, aabb.min.z},
        {aabb.min.x, aabb.max.y, aabb.min.z},
        {aabb.max.x, aabb.max.y, aabb.min.z},
        {aabb.min.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.min.y, aabb.max.z},
        {aabb.min.x, aabb.max.y, aabb.max.z},
        {aabb.max.x, aabb.max.y, aabb.max.z}
    };
    
    Polyhedron3D temp;
    auto& vertices = temp.GetVertices();
    vertices.clear();
    for (const auto& corner : corners) {
        vertices.emplace_back(corner);
    }
    
    return Intersects(&temp);
}

bool FrustumCollider::IsVisible(const Vector3f& point) const {
    return ContainsPoint(point);
}