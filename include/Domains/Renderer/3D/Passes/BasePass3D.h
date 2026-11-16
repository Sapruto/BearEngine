#pragma once

#include "Math/Matrix/Matrix4x4.h"
#include "../Materials/MaterialType.h"
#include "../Materials/Material.h"
#include "../Mesh/Mesh.h"
#include "../ForwardDeclaration.h"

#include <vector>

namespace Renderer::R3D::Passes {
    struct RenderMeshDesc {
        Materials::Material* material = nullptr;
        Matrix4x4f modelMatrix;

        RenderMeshDesc() = default;
        RenderMeshDesc(Materials::Material* m, Matrix4x4f mm) : material(m), modelMatrix(mm) {}
    };

    struct RenderBatch {
        Mesh* mesh = nullptr;
        std::vector<RenderMeshDesc> instances;

        RenderBatch() = default;
        explicit RenderBatch(Mesh* m) : mesh(m) {}
    };

    class BasePass3D {
    protected:
        Materials::MaterialType type;
        bool isInitialized = false;
        int drawLayer;

    public:
        explicit BasePass3D(Materials::MaterialType t, int layer) : type(t), drawLayer(layer) {}
        virtual ~BasePass3D() = default;

        Materials::MaterialType GetType() const { return type; }
        int GetDrawLayer() const { return drawLayer; }
        bool IsInitialized() const { return isInitialized; }

        virtual void Init() = 0;
        virtual void BeginFrame([[maybe_unused]] RenderContext3D* context) {}
        virtual void DrawFrame(const std::vector<RenderBatch>& batches) = 0;
        virtual void Shutdown() = 0;
    };
}