#pragma once

#include <unordered_map>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"
#include "FrustumCollider.h"

class ModelComponent;
class Camera3D;
class ColliderManager;

struct ObjectUVData {
    int objectID;
    std::vector<Vector3f> localVertices;
    std::vector<Vector2f> screenVertices;
    std::vector<unsigned int> indices;
    Matrix4x4f modelMatrix;
    bool isVisible;
    std::vector<float> depths;
    std::vector<Vector2f> uvCoordinates;
    bool wasVisibleLastFrame = false;
    bool isInFrustum = false;
    AABB cachedAABB;
    bool aabbValid = false;
    uint64_t lastUpdateFrame = 0;
    bool hasGeometry = false;
    
    ObjectUVData() : objectID(-1), isVisible(false), modelMatrix(Matrix4x4f::Identity()), aabbValid(false), hasGeometry(false) {}
    
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
        cachedAABB = AABB(minV, maxV);
        aabbValid = true;
    }
    
    Vector3f getCenter() const {
        if (aabbValid) {
            return (cachedAABB.min + cachedAABB.max) * 0.5f;
        }
        return Vector3f(0, 0, 0);
    }
};

class UVManager : public CollisionReaction {
private:
    std::unordered_map<int, ObjectUVData> objectUVs;
    std::unordered_map<int, bool> objectUpdateFrameState;
    std::unordered_map<int, bool> objectVisibilityState;
    std::unordered_map<int, Matrix4x4f> lastModelMatrices;
    std::unordered_map<int, uint64_t> lastUpdateFrames;
    
    Matrix4x4f m_LastViewMatrix;
    Matrix4x4f m_LastProjectionMatrix;
    
    Camera3D* camera = nullptr;
    ColliderManager* colliderManager = nullptr;
    FrustumCollider* frustumCollider = nullptr;
    bool ownFrustumCollider = false;
    
    float aspect = 1.0f;
    float fov = 60.0f;
    float near = 0.1f;
    float far = 100.0f;
    uint64_t m_FrameCounter = 0;
    
    ObjectUVData CalculateObjectUV(ModelComponent* model);
    void UpdateObjectUVWithCamera(ObjectUVData& data);
    void ProjectVertices(const std::vector<Vector3f>& vertices, const Matrix4x4f& mvp, std::vector<Vector2f>& outScreen, std::vector<float>& outDepths);
    void UpdateCache(const Matrix4x4f& view, const Matrix4x4f& proj);
    bool HasObjectMoved(ModelComponent* model, const Matrix4x4f& currentMatrix);
    bool IsObjectVisible(ModelComponent* model, ObjectUVData& data);
    Vector3f TransformPoint(const Matrix4x4f& m, const Vector3f& v);
    
    bool IsPointVisible(const Vector3f& worldPoint);
    bool IsAABBVisible(const AABB& aabb);
    
    void OnFrustumEnter(BaseCollider* self, BaseCollider* other);
    void OnFrustumExit(BaseCollider* self, BaseCollider* other);
    
public:
    UVManager();
    explicit UVManager(ColliderManager* manager);
    ~UVManager();
    
    void Initialize(ColliderManager* manager, Camera3D* camera, float aspect, float fov = 60.0f, float near = 0.1f, float far = 100.0f);
    void SetCamera(Camera3D* camera);
    void SetColliderManager(ColliderManager* manager);
    void SetFrustumCollider(FrustumCollider* collider);
    
    void UpdateUVs(const std::vector<ModelComponent*>& models);
    const ObjectUVData& GetObjectUV(ModelComponent* model);
    const std::unordered_map<int, ObjectUVData>& GetAllUVs() const { return objectUVs; }
    
    void Cleanup();
    void InvalidateCache();
    bool HasChanged() const;
    
    void SetAspect(float aspect) { this->aspect = aspect; }
    void SetFOV(float fov) { this->fov = fov; }
    void SetNearFar(float near, float far) { this->near = near; this->far = far; }
};