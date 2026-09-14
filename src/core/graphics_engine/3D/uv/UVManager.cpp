#include "UVManager.h"

#include "ModelComponent.h"
#include "Model.h"
#include "Transform3D.h"
#include "Camera3D.h"

#include <algorithm>
#include <cmath>

UVManager::UVManager() = default;
UVManager::~UVManager() = default;

void UVManager::Initialize(Camera3D* camera, float aspect,
                           float fov, float near, float far) {
    this->camera = camera;
    this->aspect = aspect;
    this->fov = fov;
    this->near = near;
    this->far = far;
    frameCacheValid = false;
}

void UVManager::SetCamera(Camera3D* camera) {
    this->camera = camera;
    frameCacheValid = false;
}

void UVManager::RebuildFrameCache() {
    if (!camera) {
        frameCacheValid = false;
        return;
    }
    viewMatrix = camera->GetViewMatrix();
    projMatrix = camera->GetProjectionMatrix(aspect);
    viewProjMatrix = projMatrix * viewMatrix;

    const float m00 = viewProjMatrix(0,0), m01 = viewProjMatrix(0,1), m02 = viewProjMatrix(0,2), m03 = viewProjMatrix(0,3);
    const float m10 = viewProjMatrix(1,0), m11 = viewProjMatrix(1,1), m12 = viewProjMatrix(1,2), m13 = viewProjMatrix(1,3);
    const float m20 = viewProjMatrix(2,0), m21 = viewProjMatrix(2,1), m22 = viewProjMatrix(2,2), m23 = viewProjMatrix(2,3);
    const float m30 = viewProjMatrix(3,0), m31 = viewProjMatrix(3,1), m32 = viewProjMatrix(3,2), m33 = viewProjMatrix(3,3);

    planes[0].normal = Vector3f(m30 + m00, m31 + m01, m32 + m02);
    planes[0].d = m33 + m03;

    planes[1].normal = Vector3f(m30 - m00, m31 - m01, m32 - m02);
    planes[1].d = m33 - m03;

    planes[2].normal = Vector3f(m30 + m10, m31 + m11, m32 + m12);
    planes[2].d = m33 + m13;

    planes[3].normal = Vector3f(m30 - m10, m31 - m11, m32 - m12);
    planes[3].d = m33 - m13;

    planes[4].normal = Vector3f(m30 + m20, m31 + m21, m32 + m22);
    planes[4].d = m33 + m23;

    planes[5].normal = Vector3f(m30 - m20, m31 - m21, m32 - m22);
    planes[5].d = m33 - m23;

    frameCacheValid = true;
}

bool UVManager::IsPointVisible(const Vector3f& worldPoint) const {
    if (!camera) return true;

    const float x = worldPoint.x;
    const float y = worldPoint.y;
    const float z = worldPoint.z;

    const float cx = viewProjMatrix(0,0)*x + viewProjMatrix(0,1)*y + viewProjMatrix(0,2)*z + viewProjMatrix(0,3);
    const float cy = viewProjMatrix(1,0)*x + viewProjMatrix(1,1)*y + viewProjMatrix(1,2)*z + viewProjMatrix(1,3);
    const float cz = viewProjMatrix(2,0)*x + viewProjMatrix(2,1)*y + viewProjMatrix(2,2)*z + viewProjMatrix(2,3);
    const float cw = viewProjMatrix(3,0)*x + viewProjMatrix(3,1)*y + viewProjMatrix(3,2)*z + viewProjMatrix(3,3);

    if (std::abs(cw) < 0.0001f) return false;
    const float invW = 1.0f / cw;
    const float nx = cx * invW;
    const float ny = cy * invW;
    const float nz = cz * invW;

    return nx >= -1.0f && nx <= 1.0f &&
           ny >= -1.0f && ny <= 1.0f &&
           nz >= -1.0f && nz <= 1.0f;
}

bool UVManager::IsAABBVisible(const UVAABB& aabb) const {
    if (!camera) return true;

    const float minX = aabb.min.x, minY = aabb.min.y, minZ = aabb.min.z;
    const float maxX = aabb.max.x, maxY = aabb.max.y, maxZ = aabb.max.z;

    for (int i = 0; i < 6; ++i) {
        const Vector3f& n = planes[i].normal;
        const float d = planes[i].d;

        const float px = (n.x >= 0.0f) ? maxX : minX;
        const float py = (n.y >= 0.0f) ? maxY : minY;
        const float pz = (n.z >= 0.0f) ? maxZ : minZ;

        if (n.x * px + n.y * py + n.z * pz + d < 0.0f) {
            return false;
        }
    }
    return true;
}

void UVManager::CalculateObjectUV(ModelComponent* model, ObjectUVData& data) {
    if (!model->IsCalculateUV()) return;

    data.objectID = model->GetID();
    data.isVisible = false;

    Transform3D* transform = model->GetGameObject()->GetComponentOfType<Transform3D>();
    if (!transform) {
        data.hasGeometry = false;
        return;
    }

    Model* mesh = model->GetModel();
    if (!mesh) {
        data.hasGeometry = false;
        data.isVisible = true;
        return;
    }

    const auto& vertices = mesh->GetVertices();
    const auto& indices = mesh->GetIndices();

    if (vertices.empty() || indices.empty()) {
        data.hasGeometry = false;
        data.isVisible = true;
        return;
    }

    data.localVertices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto& p = vertices[i].Position;
        data.localVertices[i] = Vector3f(p.x, p.y, p.z);
    }

    data.indices = indices;
    data.modelMatrix = transform->GetMatrix();
    data.hasGeometry = true;
    data.updateAABB();
    data.isVisible = true;
}

void UVManager::ProjectVertices(const std::vector<Vector3f>& vertices,
                                const Matrix4x4f& mvp,
                                std::vector<Vector2f>& outScreen,
                                std::vector<float>& outDepths) {
    const size_t n = vertices.size();
    outScreen.resize(n);
    outDepths.resize(n);

    const float m00 = mvp(0,0), m01 = mvp(0,1), m02 = mvp(0,2), m03 = mvp(0,3);
    const float m10 = mvp(1,0), m11 = mvp(1,1), m12 = mvp(1,2), m13 = mvp(1,3);
    const float m20 = mvp(2,0), m21 = mvp(2,1), m22 = mvp(2,2), m23 = mvp(2,3);
    const float m30 = mvp(3,0), m31 = mvp(3,1), m32 = mvp(3,2), m33 = mvp(3,3);

    const float nearClip = 0.001f;

    for (size_t i = 0; i < n; ++i) {
        const float x = vertices[i].x;
        const float y = vertices[i].y;
        const float z = vertices[i].z;

        const float cx = m00*x + m01*y + m02*z + m03;
        const float cy = m10*x + m11*y + m12*z + m13;
        const float cz = m20*x + m21*y + m22*z + m23;
        const float cw = m30*x + m31*y + m32*z + m33;

        if (std::abs(cw) < nearClip) {
            outScreen[i] = Vector2f(-1.0f, -1.0f);
            outDepths[i] = 1.0f;
            continue;
        }
        const float invW = 1.0f / cw;
        outScreen[i] = Vector2f(cx * invW, cy * invW);
        outDepths[i] = cz * invW;
    }
}

void UVManager::UpdateObjectUVWithCamera(ObjectUVData& data) {
    if (!camera || !data.isVisible || !data.hasGeometry || data.localVertices.empty()) {
        data.screenVertices.clear();
        data.depths.clear();
        data.uvCoordinates.clear();
        return;
    }

    const Matrix4x4f mvp = viewProjMatrix * data.modelMatrix;
    ProjectVertices(data.localVertices, mvp, data.screenVertices, data.depths);

    const size_t n = data.screenVertices.size();
    data.uvCoordinates.resize(n);
    for (size_t i = 0; i < n; ++i) {
        const Vector2f& s = data.screenVertices[i];
        data.uvCoordinates[i] = Vector2f((s.x + 1.0f) * 0.5f, (s.y + 1.0f) * 0.5f);
    }
}

void UVManager::UpdateUVs(const std::vector<ModelComponent*>& models) {
    if (!camera) return;

    RebuildFrameCache();
    ++frameCounter;

    for (ModelComponent* model : models) {
        if (!model) continue;
        GetObjectUV(model);
    }
}

const ObjectUVData& UVManager::GetObjectUV(ModelComponent* model) {
    static ObjectUVData emptyData;
    if (!model || !camera) return emptyData;

    if (!frameCacheValid) RebuildFrameCache();

    const int id = model->GetID();

    Transform3D* transform = model->GetGameObject()->GetComponentOfType<Transform3D>();
    if (!transform) return emptyData;

    const Matrix4x4f currentMatrix = transform->GetMatrix();

    auto uvIt = objectUVs.find(id);
    auto matIt = lastModelMatrices.find(id);

    const bool moved = (matIt == lastModelMatrices.end()) || (matIt->second != currentMatrix);

    if (!moved && uvIt != objectUVs.end()) {
        ObjectUVData& data = uvIt->second;
        if (data.hasGeometry) {
            const bool visible = data.aabbValid ? IsAABBVisible(data.cachedAABB) : true;
            data.isInFrustum = visible;
            data.isVisible = visible;
            data.wasVisibleLastFrame = visible;
            data.lastUpdateFrame = frameCounter;

            if (visible) {
                UpdateObjectUVWithCamera(data);
            }
        }
        return data;
    }

    ObjectUVData& data = objectUVs[id];

    CalculateObjectUV(model, data);
    data.lastUpdateFrame = frameCounter;

    if (data.isVisible) {
        const bool visible = data.hasGeometry
            ? (data.aabbValid ? IsAABBVisible(data.cachedAABB) : true)
            : IsPointVisible(transform->GetGlobalPosition());

        data.isInFrustum = visible;
        data.isVisible = visible;

        if (visible && data.hasGeometry) {
            UpdateObjectUVWithCamera(data);
        }
        else if (!visible) {
            data.clear();
        }
    }

    data.wasVisibleLastFrame = data.isVisible;
    lastModelMatrices[id] = currentMatrix;
    return data;
}

void UVManager::Cleanup() {
    std::vector<int> toRemove;
    for (const auto& [id, data] : objectUVs) {
        if (data.lastUpdateFrame != frameCounter) {
            toRemove.push_back(id);
        }
    }
    for (int id : toRemove) {
        objectUVs.erase(id);
        lastModelMatrices.erase(id);
    }
}

void UVManager::InvalidateCache() {
    frameCacheValid = false;
    frameCounter = 0;
    objectUVs.clear();
    lastModelMatrices.clear();
}

bool UVManager::HasChanged() const {
    if (!camera) return false;
    const Matrix4x4f currentView = camera->GetViewMatrix();
    const Matrix4x4f currentProj = camera->GetProjectionMatrix(aspect);
    return (currentView != viewMatrix || currentProj != projMatrix);
}