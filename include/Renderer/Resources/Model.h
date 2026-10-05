#pragma once

#include "Resource.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

#include "Vector2.h"
#include "Vector3.h"

struct Vertex {
    Vector3f Position;
    Vector3f Normal;
    Vector2f TexCoords;
    Vector3f Tangent;
    Vector3f Bitangent;
};

class Model : public Resource {
private:
    const aiScene* scene;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    
public:
    explicit Model(const std::string& path) : Resource(path) {}
    ~Model() override { Unload(); }
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override { return scene != nullptr; }

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
};