#pragma once

#include "Shared/Resource/Resource.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"

struct Vertex {
    Vector3f Position;
    Vector3f Normal;
    Vector2f TexCoords;
    Vector3f Tangent;
    Vector3f Bitangent;
};

class Model : public Resource {
private:
    Assimp::Importer importer;
    const aiScene* scene{nullptr};

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    static unsigned int nextID;
    unsigned int id;

    unsigned int VAO = 0, VBO = 0, IBO = 0;

    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh);

    void CreateBuffers();
    void DeleteBuffers();
    
public:
    explicit Model(const std::string& path) : Resource(path) {}
    ~Model() override { Unload(); }
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override { return scene != nullptr; }

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
};