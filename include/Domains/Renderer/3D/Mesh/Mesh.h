#pragma once

#include <string>
#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

#include "Systems/Resource/Resource.h"
#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"

#include "RHI/Base/Handles.h"

struct MeshVertex {
    Vector3f position;
    Vector3f normal;
    Vector2f texCoords;
    Vector3f tangent;
    Vector3f bitangent;
};

class Mesh : public Resource {
private:
    Assimp::Importer importer;
    const aiScene* scene{nullptr};

    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;

    RHI::Handles::BufferHandle vboHandle;
    RHI::Handles::BufferHandle iboHandle;

    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh);

    void CreateBuffers();
    void DeleteBuffers();
    
public:
    explicit Mesh(const std::string& path);
    ~Mesh() override { Unload(); }
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override { return scene != nullptr; }

    const std::vector<MeshVertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }

    RHI::Handles::BufferHandle GetVBO() const { return vboHandle; }
    RHI::Handles::BufferHandle GetIBO() const { return iboHandle; }
};