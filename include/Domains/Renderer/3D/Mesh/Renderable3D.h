#pragma once

#include "Mesh.h"
#include "../Passes/DefaultPass.h"
#include "../Materials/Material.h"
#include "../../Base/IRenderable.h"

#include "Systems/Core/HasRequired.h"

#include "Engine/Serialization/SerializeField.h"

#include "Math/Transform/Transform3D.h"
#include "Math/Matrix/Matrix4x4.h"

#include <atomic>
#include <string>
#include <vector>
#include <memory>

class RendererScene;

namespace Renderer::R3D {
    class Renderable3D : public IRenderable, public Systems::HasRequired {
    private:
        uint32_t id;

        Mesh* mesh{nullptr};
        FIELD(std::string, meshPath);

        Transform3D* transform;

        std::vector<std::unique_ptr<Materials::Material>> materials;
        
        static std::atomic<uint32_t> nextId;

    public:
        Renderable3D(const std::string& meshPath);
        Renderable3D() : id(nextId.fetch_add(1)) {}

        void Awake() override;
        void Start() override;

        void SetMeshPath(const std::string& newMeshPath) { meshPath = newMeshPath; }

        Mesh* GetMesh() const { return mesh; }
        const std::string& GetMeshPath() const { return meshPath.GetValue(); }

        Materials::Material* AddMaterial(std::unique_ptr<Materials::Material> material) {
            Materials::Material* raw = material.get();
            materials.push_back(std::move(material));
            return raw;
        }

        Materials::Material* GetMaterialOfType(Materials::MaterialType type) {
            for (auto& material : materials) {
                if (material->GetType() == type)  return material.get();
            }
            return nullptr;
        }

        template<typename T>
        T* GetMaterialOfType() {
            for (auto& material : materials) {
                if (auto* cast = dynamic_cast<T*>(material.get())) {
                    return cast;
                }
            }
            return nullptr;
        }

        template<typename T>
        const T* GetMaterialOfType() const {
            for (const auto& material : materials) {
                if (auto* cast = dynamic_cast<const T*>(material.get())) {
                    return cast;
                }
            }
            return nullptr;
        }

        const std::vector<std::unique_ptr<Materials::Material>>& GetMaterials() const {
            return materials;
        }

        std::size_t MaterialsCount() const { return materials.size(); }
        bool HasMaterials() const { return !materials.empty(); }

        void ClearMaterial() { materials.clear(); }

        unsigned int GetID() const { return id; }

        Matrix4x4f GetMatrix() {
            if (!gameObject) return Matrix4x4f::Zero();
            if(!transform) {
                transform = gameObject->GetComponentOfType<Transform3D>();
                if (!transform) {
                    return Matrix4x4f::Zero();
                }
            }
            return transform->GetMatrix();
        }

        SERIALIZED_FIELDS(&meshPath)
    };
}