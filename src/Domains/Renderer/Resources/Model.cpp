#include "Model.h"
#include <iostream>

#include <fstream>
#include <iostream>

#include <cerrno>
#include <cstring>

Assimp::Importer importer;

bool Model::Load() {
    scene = importer.ReadFile(pathName, 
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_GenNormals |     
        aiProcess_OptimizeMeshes);
        
    if (!scene) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }
    
    vertices.clear();
    indices.clear();
    ProcessNode(scene->mRootNode, scene);
    
    std::cout << "Assimp loaded successfully: " 
              << vertices.size() << " vertices, " 
              << indices.size() << " indices" << std::endl;
    return true;
}

void Model::Unload() {
    vertices.clear();
    indices.clear();
    scene = nullptr;
    
    importer.FreeScene();
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    unsigned int vertexOffset = vertices.size();
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;
        
        if (mesh->HasNormals()) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        } 
        else {
            vertex.Normal = Vector3f(0.0f, 1.0f, 0.0f);
        }
        
        if (mesh->HasTextureCoords(0)) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        } 
        else {
            vertex.TexCoords = Vector2f::Zero();
        }
        
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent.x = mesh->mTangents[i].x;
            vertex.Tangent.y = mesh->mTangents[i].y;
            vertex.Tangent.z = mesh->mTangents[i].z;
            
            vertex.Bitangent.x = mesh->mBitangents[i].x;
            vertex.Bitangent.y = mesh->mBitangents[i].y;
            vertex.Bitangent.z = mesh->mBitangents[i].z;
        } 
        else {
            vertex.Tangent = Vector3f::Zero();
            vertex.Bitangent = Vector3f::Zero();
        }
        
        vertices.push_back(vertex);
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j] + vertexOffset);
        }
    }
}