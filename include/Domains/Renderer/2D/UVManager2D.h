#pragma once

#include <unordered_map>
#include <cstdint>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"

class SpriteComponent;
class Camera2D;

struct SpriteUVData {
    int spriteID;
    std::vector<Vector2f> vertices;
    std::vector<Vector2f> screenVertices;
    std::vector<unsigned int> indices;
    Matrix4x4f modelMatrix;
    bool isVisible;
    std::vector<Vector2f> uvCoordinates;
    bool isInFrustum;
    
    SpriteUVData() 
        : spriteID(-1), 
          isVisible(false), 
          isInFrustum(false),
          modelMatrix(Matrix4x4f::Identity()) {}
    
    void clear() {
        vertices.clear();
        screenVertices.clear();
        indices.clear();
        uvCoordinates.clear();
        isVisible = false;
        isInFrustum = false;
    }
};

class UVManager2D {
private:
    std::unordered_map<int, SpriteUVData> spriteUVs;
    std::unordered_map<int, bool> updateState;
    std::unordered_map<int, Matrix4x4f> lastMatrices;
    
    Camera2D* camera = nullptr;
    float aspect = 1.0f;
    uint64_t frameCounter = 0;
    
    SpriteUVData CalculateSpriteUV(SpriteComponent* sprite);
    void UpdateSpriteUVWithCamera(SpriteUVData& data);
    bool HasSpriteMoved(SpriteComponent* sprite, const Matrix4x4f& currentMatrix);
    bool IsSpriteVisible(SpriteComponent* sprite, SpriteUVData& data);
    
public:
    UVManager2D() = default;
    ~UVManager2D() = default;
    
    void SetCamera(Camera2D* cam) { camera = cam; }
    void SetAspect(float a) { aspect = a; }
    
    void UpdateUVs(const std::vector<SpriteComponent*>& sprites);
    const SpriteUVData& GetSpriteUV(SpriteComponent* sprite);
    
    void InvalidateCache();
    void Cleanup();
    
    size_t GetCacheSize() const { return spriteUVs.size(); }
    bool IsSpriteCached(int spriteID) const { return spriteUVs.find(spriteID) != spriteUVs.end(); }
};