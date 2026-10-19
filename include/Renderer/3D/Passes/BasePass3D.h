#pragma once

#include "Math/Matrix/Matrix4x4.h"
#include "Renderer/3D/Materials/MaterialType.h"
#include "Renderer/3D/Materials/Material.h"
#include "Renderer/3D/Mesh/Mesh.h"

#include <vector>

class RenderContext3D;

struct RenderMeshDesc {
    Mesh* mesh;
    Material* material;
    Matrix4x4f modelMatrix;

    RenderMeshDesc() = default;
    RenderMeshDesc(Mesh* newMesh, Material* newMaterial, Matrix4x4f newModelMatrix) :
                    mesh(newMesh),
                    material(newMaterial),
                    modelMatrix(newModelMatrix) {}
};

class BasePass3D {
protected:
    MaterialType type;
    bool isInitialized = false;
    int drawLayer;

public:
    explicit BasePass3D(MaterialType t, int layer)
        : type(t), drawLayer(layer) {}
    virtual ~BasePass3D() = default;
    
    MaterialType GetType() const { return type; }
    int GetDrawLayer() const { return drawLayer; }
    bool IsInitialized() const { return isInitialized; }
    
    virtual void Init() = 0;
    virtual void BeginFrame([[maybe_unused]] RenderContext3D* context) {}
    virtual void DrawFrame(const std::vector<RenderMeshDesc>& descs) = 0;
    virtual void Shutdown() = 0;
};