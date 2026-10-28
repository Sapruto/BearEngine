#pragma once

#include "Mesh.h"
#include "../Passes/DefaultPass.h"
#include "../Materials/Material.h"
#include "../../Base/IRenderable.h"

#include "Systems/Core/HasRequired.h"
#include "Systems/Resource/ResourceSystem.h"

#include "Engine/Serialization/SerializeField.h"

#include "Math/Transform/Transform3D.h"
#include "Math/Matrix/Matrix4x4.h"

#include <atomic>
#include <string>
#include <vector>
#include <memory>

class RendererScene;

class Renderable3D : public IRenderable, public Systems::HasRequired {
private:
    static std::atomic<unsigned> nextId;
    unsigned int id;

    ResourceSystem* resourceSystem{nullptr};
    Mesh* mesh{nullptr};
    FIELD(std::string, meshPath);

    Transform3D* transform;

    std::vector<std::unique_ptr<Material>> materials;

public:
    Renderable3D(ResourceSystem& resource, const std::string& meshPath);
    Renderable3D() : id(nextId.fetch_add(1)) {}

    void Awake() override;
    void Start() override;

    void SetMeshPath(const std::string& newMeshPath) { meshPath = newMeshPath; }

    Mesh* GetMesh() const { return mesh; }
    const std::string& GetMeshPath() const { return meshPath.GetValue(); }

    Material* AddMaterial(std::unique_ptr<Material> material) {
        Material* raw = material.get();
        materials.push_back(std::move(material));
        return raw;
    }

    Material* GetMaterialOfType(MaterialType type) {
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

    std::vector<Material*> GetMaterials() {
        std::vector<Material*> result;
        result.reserve(materials.size());
        for (auto& material : materials) {
            result.push_back(material.get());
        }
        return result;
    }

    std::vector<const Material*> GetMaterials() const {
        std::vector<const Material*> result;
        result.reserve(materials.size());
        for (const auto& material : materials) {
            result.push_back(material.get());
        }
        return result;
    }

    std::size_t GetMaterialCount() const { return materials.size(); }
    bool HasMaterials() const { return !materials.empty(); }

    void ClearMaterials() { materials.clear(); }

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