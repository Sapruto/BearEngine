#pragma once

#include "Resource.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
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