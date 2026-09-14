#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"

class ModelComponent;
class Camera3D;

struct UVAABB {
    Vector3f min{ 0.0f, 0.0f, 0.0f };
    Vector3f max{ 0.0f, 0.0f, 0.0f };

    UVAABB() = default;
    UVAABB(const Vector3f& mn, const Vector3f& mx) : min(mn), max(mx) {}

    Vector3f center() const {
        return (min + max) * 0.5f;
    }

    Vector3f extents() const {
        return (max - min) * 0.5f;
    }
};

struct ObjectUVData {
    int objectID = -1;
    std::vector<Vector3f> localVertices;
    std::vector<Vector2f> screenVertices;
    std::vector<unsigned int> indices;
    Matrix4x4f modelMatrix = Matrix4x4f::Identity();
    bool isVisible = false;
    std::vector<float> depths;
    std::vector<Vector2f> uvCoordinates;
    bool wasVisibleLastFrame = false;
    bool isInFrustum = false;
    UVAABB cachedAABB;
    bool aabbValid = false;
    uint64_t lastUpdateFrame = 0;
    bool hasGeometry = false;

    ObjectUVData() = default;

    void clear() {
        localVertices.clear();
        screenVertices.clear();
        indices.clear();
        depths.clear();
        uvCoordinates.clear();
        isVisible = false;
        aabbValid = false;
        hasGeometry = false;
    }

    void reserve(size_t vertexCount, size_t indexCount) {
        localVertices.reserve(vertexCount);
        screenVertices.reserve(vertexCount);
        depths.reserve(vertexCount);
        indices.reserve(indexCount);
        uvCoordinates.reserve(vertexCount);
        hasGeometry = (vertexCount > 0 && indexCount > 0);
    }

    void updateAABB() {
        if (localVertices.empty()) {
            aabbValid = false;
            return;
        }
        Vector3f minV = localVertices[0];
        Vector3f maxV = localVertices[0];
        for (const auto& v : localVertices) {
            minV.x = std::min(minV.x, v.x);
            minV.y = std::min(minV.y, v.y);
            minV.z = std::min(minV.z, v.z);
            maxV.x = std::max(maxV.x, v.x);
            maxV.y = std::max(maxV.y, v.y);
            maxV.z = std::max(maxV.z, v.z);
        }
        cachedAABB = UVAABB(minV, maxV);
        aabbValid = true;
    }

    Vector3f getCenter() const {
        if (aabbValid) {
            return cachedAABB.center();
        }
        return Vector3f(0, 0, 0);
    }
};

struct UVPlane {
    Vector3f normal;
    float d = 0.0f;
};

class UVManager {
private:
    std::unordered_map<int, ObjectUVData> objectUVs;
    std::unordered_map<int, Matrix4x4f> lastModelMatrices;

    Camera3D* camera = nullptr;

    float aspect = 1.0f;
    float fov = 60.0f;
    float near = 0.1f;
    float far = 100.0f;
    uint64_t frameCounter = 0;

    UVPlane planes[6];

    Matrix4x4f viewMatrix = Matrix4x4f::Identity();
    Matrix4x4f projMatrix = Matrix4x4f::Identity();
    Matrix4x4f viewProjMatrix = Matrix4x4f::Identity();
    bool frameCacheValid = false;

    void CalculateObjectUV(ModelComponent* model, ObjectUVData& data);
    void UpdateObjectUVWithCamera(ObjectUVData& data);
    void ProjectVertices(const std::vector<Vector3f>& vertices,
                         const Matrix4x4f& mvp,
                         std::vector<Vector2f>& outScreen,
                         std::vector<float>& outDepths);

    bool IsAABBVisible(const UVAABB& aabb) const;
    bool IsPointVisible(const Vector3f& worldPoint) const;

    void RebuildFrameCache();

public:
    UVManager();
    ~UVManager();

    void Initialize(Camera3D* camera, float aspect,
                    float fov = 60.0f, float near = 0.1f, float far = 100.0f);
    void SetCamera(Camera3D* camera);

    void UpdateUVs(const std::vector<ModelComponent*>& models);
    const ObjectUVData& GetObjectUV(ModelComponent* model);
    const std::unordered_map<int, ObjectUVData>& GetAllUVs() const { return objectUVs; }

    void Cleanup();
    void InvalidateCache();
    bool HasChanged() const;

    void SetAspect(float aspect) { this->aspect = aspect; frameCacheValid = false; }
    void SetFOV(float fov) { this->fov = fov; frameCacheValid = false; }
    void SetNearFar(float n, float f) { near = n; far = f; frameCacheValid = false; }
};