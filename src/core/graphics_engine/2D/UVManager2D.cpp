#include "UVManager2D.h"

#include "SpriteComponent.h"
#include "Transform2D.h"
#include "Camera2D.h"
#include "GameObject.h"
#include <cmath>

void UVManager2D::UpdateUVs(const std::vector<SpriteComponent*>& sprites) {
    if (!camera) return;
    
    for (auto& [id, state] : updateState) {
        state = false;
    }
    
    for (auto* sprite : sprites) {
        if (!sprite) continue;
        
        int id = sprite->GetID();
        GameObject* go = sprite->GetGameObject();
        if (!go) continue;
        
        Transform2D* transform = go->GetComponentOfType<Transform2D>();
        if (!transform) continue;
        
        Matrix4x4f currentMatrix = transform->GetMatrix();
        bool moved = HasSpriteMoved(sprite, currentMatrix);
        
        if (moved || spriteUVs.find(id) == spriteUVs.end()) {
            SpriteUVData data = CalculateSpriteUV(sprite);
            
            bool visible = IsSpriteVisible(sprite, data);
            data.isInFrustum = visible;
            data.isVisible = visible;
            
            spriteUVs[id] = data;
            lastMatrices[id] = currentMatrix;
        }
        else {
            auto& data = spriteUVs[id];
            data.isVisible = IsSpriteVisible(sprite, data);
            data.isInFrustum = data.isVisible;
        }
        
        updateState[id] = true;
    }
    
    Cleanup();
    
    frameCounter++;
}

const SpriteUVData& UVManager2D::GetSpriteUV(SpriteComponent* sprite) {
    static SpriteUVData emptyData;
    
    if (!sprite || !camera) return emptyData;
    
    int id = sprite->GetID();
    auto it = spriteUVs.find(id);
    
    if (it != spriteUVs.end() && updateState[id]) {
        return it->second;
    }
    
    return emptyData;
}

void UVManager2D::InvalidateCache() {
    spriteUVs.clear();
    updateState.clear();
    lastMatrices.clear();
}

void UVManager2D::Cleanup() {
    std::vector<int> toRemove;
    
    for (const auto& [id, _] : spriteUVs) {
        auto stateIt = updateState.find(id);
        if (stateIt == updateState.end() || !stateIt->second) {
            toRemove.push_back(id);
        }
    }
    
    for (int id : toRemove) {
        spriteUVs.erase(id);
        updateState.erase(id);
        lastMatrices.erase(id);
    }
}

SpriteUVData UVManager2D::CalculateSpriteUV(SpriteComponent* sprite) {
    SpriteUVData data;
    data.spriteID = sprite->GetID();
    data.isVisible = false;
    data.isInFrustum = false;
    
    GameObject* go = sprite->GetGameObject();
    if (!go) return data;
    
    Transform2D* transform = go->GetComponentOfType<Transform2D>();
    if (!transform) return data;
    
    float w = sprite->GetWidth() * 0.5f;
    float h = sprite->GetHeight() * 0.5f;
    
    data.vertices = {
        Vector2f(-w,  h),
        Vector2f( w,  h),
        Vector2f( w, -h),
        Vector2f(-w, -h)
    };
    
    data.uvCoordinates = {
        Vector2f(0.0f, 1.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(1.0f, 0.0f),
        Vector2f(0.0f, 0.0f)
    };
    
    data.indices = {0, 1, 2, 0, 2, 3};
    data.modelMatrix = transform->GetMatrix();
    
    if (camera) {
        data.screenVertices.clear();
        data.screenVertices.reserve(data.vertices.size());
        
        for (const auto& vertex : data.vertices) {
            Vector2f worldPos = Vector2f(
                data.modelMatrix(0,0) * vertex.x + data.modelMatrix(0,1) * vertex.y + data.modelMatrix(0,3),
                data.modelMatrix(1,0) * vertex.x + data.modelMatrix(1,1) * vertex.y + data.modelMatrix(1,3)
            );
            
            data.screenVertices.push_back(worldPos);
        }
    }
    
    data.isVisible = true;
    return data;
}

void UVManager2D::UpdateSpriteUVWithCamera(SpriteUVData& data) {
    if (!camera || !data.isVisible) return;
}

bool UVManager2D::HasSpriteMoved(SpriteComponent* sprite, const Matrix4x4f& currentMatrix) {
    int id = sprite->GetID();
    auto it = lastMatrices.find(id);
    if (it == lastMatrices.end()) return true;
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float diff = std::abs(it->second(i, j) - currentMatrix(i, j));
            if (diff > 0.0001f) {
                return true;
            }
        }
    }
    return false;
}

bool UVManager2D::IsSpriteVisible(SpriteComponent* sprite, SpriteUVData& data) {
    if (!camera || !sprite) return false;
    
    Vector2f worldPos = Vector2f(
        data.modelMatrix(0, 3),
        data.modelMatrix(1, 3)
    );
    
    float halfWidth = sprite->GetWidth() * 0.5f;
    float halfHeight = sprite->GetHeight() * 0.5f;
    
    float orthoSize = camera->GetOrthoSize();
    float aspect = camera->GetAspect();
    float halfWidthCam = orthoSize * aspect;
    float halfHeightCam = orthoSize;
    
    Transform2D* camTransform = camera->GetTransform();
    if (!camTransform) return false;
    
    Vector2f camPos = camTransform->GetPosition();
    
    float left = worldPos.x - halfWidth;
    float right = worldPos.x + halfWidth;
    float top = worldPos.y + halfHeight;
    float bottom = worldPos.y - halfHeight;
    
    float camLeft = camPos.x - halfWidthCam;
    float camRight = camPos.x + halfWidthCam;
    float camTop = camPos.y + halfHeightCam;
    float camBottom = camPos.y - halfHeightCam;
    
    if (right < camLeft || left > camRight || 
        top < camBottom || bottom > camTop) {
        return false;
    }
    
    return true;
}