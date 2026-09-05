#include "UVManager.h"

#include "ModelComponent.h"
#include "Model.h"
#include "Transform3D.h"
#include "Camera3D.h"
#include "ColliderManager.h"
#include "Polyhedron3D.h"
#include <algorithm>
#include <cmath>

UVManager::UVManager() {
    auto enterHandler = [this](BaseCollider* self, BaseCollider* other) {
        OnFrustumEnter(self, other);
    };
    auto exitHandler = [this](BaseCollider* self, BaseCollider* other) {
        OnFrustumExit(self, other);
    };
    
    Subscribe(enterHandler, CollisionEvent::State::ENTER);
    Subscribe(exitHandler, CollisionEvent::State::EXIT);
}

UVManager::UVManager(ColliderManager* manager) : UVManager() {
    Initialize(manager, nullptr, 1.0f);

    auto enterHandler = [this](BaseCollider* self, BaseCollider* other) {
        OnFrustumEnter(self, other);
    };
    auto exitHandler = [this](BaseCollider* self, BaseCollider* other) {
        OnFrustumExit(self, other);
    };
    
    Subscribe(enterHandler, CollisionEvent::State::ENTER);
    Subscribe(exitHandler, CollisionEvent::State::EXIT);
}

UVManager::~UVManager() {
    if (ownFrustumCollider && frustumCollider) {
        delete frustumCollider;
        frustumCollider = nullptr;
    }
}

void UVManager::Initialize(ColliderManager* manager, Camera3D* camera, float aspect, float fov, float near, float far) {
    this->colliderManager = manager;
    this->camera = camera;
    this->aspect = aspect;
    this->fov = fov;
    this->near = near;
    this->far = far;
    
    if (colliderManager && !frustumCollider) {
        frustumCollider = new FrustumCollider(camera, aspect, fov, near, far);
        colliderManager->AddCollider(frustumCollider);
        ownFrustumCollider = true;
    }
}

void UVManager::SetCamera(Camera3D* camera) {
    this->camera = camera;
    if (frustumCollider) {
        frustumCollider->SetCamera(camera);
    }
}

void UVManager::SetColliderManager(ColliderManager* manager) {
    this->colliderManager = manager;
    if (frustumCollider && colliderManager) {
        colliderManager->AddCollider(frustumCollider);
    }
}

void UVManager::SetFrustumCollider(FrustumCollider* collider) {
    if (ownFrustumCollider && frustumCollider) {
        if (colliderManager) {
            colliderManager->RemoveCollider(frustumCollider);
        }
        delete frustumCollider;
    }
    frustumCollider = collider;
    ownFrustumCollider = false;
}

bool UVManager::IsPointVisible(const Vector3f& worldPoint) {
    if (!camera) return true;
    
    Matrix4x4f view = camera->GetViewMatrix();
    Matrix4x4f proj = camera->GetProjectionMatrix(aspect);
    Matrix4x4f mvp = proj * view;
    
    Vector3f clipPos = TransformPoint(mvp, worldPoint);
    
    return clipPos.x >= -1.0f && clipPos.x <= 1.0f &&
           clipPos.y >= -1.0f && clipPos.y <= 1.0f &&
           clipPos.z >= -1.0f && clipPos.z <= 1.0f;
}

bool UVManager::IsAABBVisible(const AABB& aabb) {
    if (!frustumCollider) return true;
    return frustumCollider->IsVisible(aabb);
}

void UVManager::OnFrustumEnter(BaseCollider* self, BaseCollider* other) {
    GameObject* go = other->GetGameObject();
    if (!go) return;
    
    ModelComponent* model = go->GetComponentOfType<ModelComponent>();
    if (!model) return;
    
    int id = model->GetID();
    objectVisibilityState[id] = true;
    
    auto it = objectUVs.find(id);
    if (it != objectUVs.end()) {
        it->second.isInFrustum = true;
        it->second.isVisible = true;
    }
}

void UVManager::OnFrustumExit(BaseCollider* self, BaseCollider* other) {
    GameObject* go = other->GetGameObject();
    if (!go) return;
    
    ModelComponent* model = go->GetComponentOfType<ModelComponent>();
    if (!model) return;
    
    int id = model->GetID();
    objectVisibilityState[id] = false;
    
    auto it = objectUVs.find(id);
    if (it != objectUVs.end()) {
        it->second.isInFrustum = false;
        it->second.isVisible = false;
    }
}

Vector3f UVManager::TransformPoint(const Matrix4x4f& m, const Vector3f& v) {
    float w = m(3,0) * v.x + m(3,1) * v.y + m(3,2) * v.z + m(3,3);
    if (std::abs(w) < 0.0001f) return Vector3f(0, 0, 0);
    return Vector3f(
        (m(0,0) * v.x + m(0,1) * v.y + m(0,2) * v.z + m(0,3)) / w,
        (m(1,0) * v.x + m(1,1) * v.y + m(1,2) * v.z + m(1,3)) / w,
        (m(2,0) * v.x + m(2,1) * v.y + m(2,2) * v.z + m(2,3)) / w
    );
}

bool UVManager::HasObjectMoved(ModelComponent* model, const Matrix4x4f& currentMatrix) {
    int id = model->GetID();
    auto it = lastModelMatrices.find(id);
    if (it == lastModelMatrices.end()) return true;
    return it->second != currentMatrix;
}

bool UVManager::IsObjectVisible(ModelComponent* model, ObjectUVData& data) {
    if (!frustumCollider) return true;
    
    auto visIt = objectVisibilityState.find(model->GetID());
    if (visIt != objectVisibilityState.end()) {
        return visIt->second;
    }
    
    Transform3D* transform = model->GetGameObject()->GetComponentOfType<Transform3D>();
    if (!transform) return false;
    
    Polyhedron3D* polyhedron = model->GetGameObject()->GetComponentOfType<Polyhedron3D>();
    if (polyhedron) {
        bool visible = frustumCollider->Intersects(polyhedron);
        objectVisibilityState[model->GetID()] = visible;
        return visible;
    }
    
    if (data.hasGeometry) {
        if (!data.aabbValid) {
            data.updateAABB();
        }
        if (data.aabbValid) {
            bool visible = IsAABBVisible(data.cachedAABB);
            objectVisibilityState[model->GetID()] = visible;
            return visible;
        }
    }
    
    Vector3f worldPos = transform->GetGlobalPosition();
    bool visible = IsPointVisible(worldPos);
    objectVisibilityState[model->GetID()] = visible;
    return visible;
}

ObjectUVData UVManager::CalculateObjectUV(ModelComponent* model) {
    ObjectUVData data;
    data.objectID = model->GetID();
    data.isVisible = false;
    
    Transform3D* transform = model->GetGameObject()->GetComponentOfType<Transform3D>();
    if (!transform) return data;
    
    Model* mesh = model->GetModel();
    if (!mesh) {
        data.hasGeometry = false;
        data.isVisible = true;
        return data;
    }
    
    const auto& vertices = mesh->GetVertices();
    const auto& indices = mesh->GetIndices();
    
    if (vertices.empty() || indices.empty()) {
        data.hasGeometry = false;
        data.isVisible = true;
        return data;
    }
    
    data.reserve(vertices.size(), indices.size());
    
    for (const auto& vertex : vertices) {
        data.localVertices.emplace_back(vertex.Position.x, vertex.Position.y, vertex.Position.z);
    }
    
    data.indices = indices;
    data.modelMatrix = transform->GetMatrix();
    data.hasGeometry = true;
    data.updateAABB();
    data.isVisible = true;
    
    return data;
}

void UVManager::ProjectVertices(const std::vector<Vector3f>& vertices, const Matrix4x4f& mvp, std::vector<Vector2f>& outScreen, std::vector<float>& outDepths) {
    outScreen.clear();
    outDepths.clear();
    outScreen.reserve(vertices.size());
    outDepths.reserve(vertices.size());
    
    const float nearClip = 0.001f;
    
    for (const auto& localPos : vertices) {
        Vector3f clipPos = TransformPoint(mvp, localPos);
        if (std::abs(clipPos.z) < nearClip) {
            outScreen.emplace_back(-1.0f, -1.0f);
            outDepths.push_back(1.0f);
            continue;
        }
        float invW = 1.0f / clipPos.z;
        outScreen.emplace_back(clipPos.x * invW, clipPos.y * invW);
        outDepths.push_back(clipPos.z);
    }
}

void UVManager::UpdateObjectUVWithCamera(ObjectUVData& data) {
    if (!camera || !data.isVisible) return;
    
    if (!data.hasGeometry || data.localVertices.empty()) {
        data.screenVertices.clear();
        data.depths.clear();
        data.uvCoordinates.clear();
        return;
    }
    
    Matrix4x4f view = camera->GetViewMatrix();
    Matrix4x4f proj = camera->GetProjectionMatrix(aspect);
    Matrix4x4f mvp = proj * view * data.modelMatrix;
    
    ProjectVertices(data.localVertices, mvp, data.screenVertices, data.depths);
    
    data.uvCoordinates.clear();
    data.uvCoordinates.reserve(data.screenVertices.size());
    
    for (const auto& screenPos : data.screenVertices) {
        data.uvCoordinates.emplace_back((screenPos.x + 1.0f) * 0.5f, (screenPos.y + 1.0f) * 0.5f);
    }
}

void UVManager::UpdateCache(const Matrix4x4f& view, const Matrix4x4f& proj) {
    if (view != m_LastViewMatrix || proj != m_LastProjectionMatrix) {
        m_LastViewMatrix = view;
        m_LastProjectionMatrix = proj;
    }
}

void UVManager::UpdateUVs(const std::vector<ModelComponent*>& models) {
    if (!camera || !colliderManager) return;
    
    if (frustumCollider) {
        frustumCollider->SetAspect(aspect);
        frustumCollider->SetFOV(fov);
        frustumCollider->SetNearFar(near, far);
        frustumCollider->Update();
    }
    
    Matrix4x4f view = camera->GetViewMatrix();
    Matrix4x4f proj = camera->GetProjectionMatrix(aspect);
    UpdateCache(view, proj);
    
    m_FrameCounter++;
    for (auto* model : models) {
        if (model) {
            objectUpdateFrameState[model->GetID()] = false;
        }
    }
}

const ObjectUVData& UVManager::GetObjectUV(ModelComponent* model) {
    static ObjectUVData emptyData;
    if (!model || !camera) return emptyData;
    
    int id = model->GetID();
    
    auto frameIt = lastUpdateFrames.find(id);
    if (frameIt != lastUpdateFrames.end() && frameIt->second == m_FrameCounter) {
        auto uvIt = objectUVs.find(id);
        if (uvIt != objectUVs.end()) {
            return uvIt->second;
        }
        return emptyData;
    }
    
    auto stateIt = objectUpdateFrameState.find(id);
    if (stateIt != objectUpdateFrameState.end() && stateIt->second) {
        auto uvIt = objectUVs.find(id);
        if (uvIt != objectUVs.end()) {
            return uvIt->second;
        }
        return emptyData;
    }
    
    Transform3D* transform = model->GetGameObject()->GetComponentOfType<Transform3D>();
    if (!transform) return emptyData;
    
    Matrix4x4f currentMatrix = transform->GetMatrix();
    bool moved = HasObjectMoved(model, currentMatrix);
    
    ObjectUVData data;
    auto uvIt = objectUVs.find(id);
    if (uvIt != objectUVs.end() && !moved) {
        data = uvIt->second;
    } 
    else {
        data = CalculateObjectUV(model);
    }
    
    objectUpdateFrameState[id] = true;
    lastUpdateFrames[id] = m_FrameCounter;
    
    if (data.isVisible) {
        bool visible = IsObjectVisible(model, data);
        data.isInFrustum = visible;
        data.isVisible = visible;
        
        if (visible && moved && data.hasGeometry) {
            UpdateObjectUVWithCamera(data);
        } 
        else if (!visible) {
            data.clear();
        }
    }
    
    data.wasVisibleLastFrame = data.isVisible;
    lastModelMatrices[id] = currentMatrix;
    objectUVs[id] = std::move(data);
    return objectUVs[id];
}

void UVManager::Cleanup() {
    std::vector<int> toRemove;
    for (const auto& [id, data] : objectUVs) {
        if (objectUpdateFrameState.find(id) == objectUpdateFrameState.end()) {
            toRemove.push_back(id);
        }
    }
    for (int id : toRemove) {
        objectUVs.erase(id);
        objectVisibilityState.erase(id);
        lastModelMatrices.erase(id);
        lastUpdateFrames.erase(id);
    }
}

void UVManager::InvalidateCache() {
    m_LastViewMatrix = Matrix4x4f::Identity();
    m_LastProjectionMatrix = Matrix4x4f::Identity();
    m_FrameCounter = 0;
    objectVisibilityState.clear();
    lastModelMatrices.clear();
    lastUpdateFrames.clear();
}

bool UVManager::HasChanged() const {
    if (!camera) return false;
    Matrix4x4f currentView = camera->GetViewMatrix();
    Matrix4x4f currentProj = camera->GetProjectionMatrix(aspect);
    return (currentView != m_LastViewMatrix || currentProj != m_LastProjectionMatrix);
}