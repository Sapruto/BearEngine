#pragma once

#include "Shared/Resource/Resource.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"

#include "RHI/Base/Handles.h"

struct Vertex {
    Vector3f Position;
    Vector3f Normal;
    Vector2f TexCoords;
    Vector3f Tangent;
    Vector3f Bitangent;
};

class Mesh : public Resource {
private:
    Assimp::Importer importer;
    const aiScene* scene{nullptr};

    std::vector<Vertex> vertices;
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

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }

    RHI::Handles::BufferHandle GetVBO() const { return vboHandle; }
    RHI::Handles::BufferHandle GetIBO() const { return iboHandle; }
};