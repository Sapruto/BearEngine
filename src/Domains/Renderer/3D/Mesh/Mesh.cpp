#include "Domains/Renderer/3D/Mesh/Mesh.h"

#include <iostream>

#include <cerrno>
#include <cstring>

#include <span>

#include "RHI/RuntimeRHI.h"
#include "RHI/MacrosRHI.h"

#include "RHI/Base/Contracts/ResourceExecutorParams.h"
#include "RHI/Base/Contracts/ResourceExecutorResult.h"

#include "Domains/Renderer/RenderBackendAllias.h"

#include "RHI/RuntimeRHI.h"

using namespace RHI;
using namespace RHI::Executors;
using namespace RHI::Executors::ResourceExecutor;
using namespace RHI::FabricRHI;

using namespace Renderer::Base;

Mesh::Mesh(const std::string& path) : Resource(path) {}

void Mesh::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Mesh::ProcessMesh(aiMesh* mesh) {
    unsigned int vertexOffset = vertices.size();
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        MeshVertex vertex;
        
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        
        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        } 
        else {
            vertex.normal = Vector3f(0.0f, 1.0f, 0.0f);
        }
        
        if (mesh->HasTextureCoords(0)) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } 
        else {
            vertex.texCoords = Vector2f::Zero();
        }
        
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
            
            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        } 
        else {
            vertex.tangent = Vector3f::Zero();
            vertex.bitangent = Vector3f::Zero();
        }
        
        vertices.push_back(vertex);
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j] + vertexOffset);
        }
    }

    std::cout << "[vertex] pos=(" << vertices[0].position.x << ","
            << vertices[0].position.y << ","
            << vertices[0].position.z << ")"
            << " sizeof(Vector3f)=" << sizeof(Vector3f)
            << " sizeof(MeshVertex)=" << sizeof(MeshVertex)
            << " offsetof(pos)=" << offsetof(MeshVertex, position)
            << " offsetof(normal)=" << offsetof(MeshVertex, normal) << "\n";
}

void Mesh::CreateBuffers() {
    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* device = rhi.Device();
    auto* executorsBase = rhi.Executors();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(executorsBase);

    auto* resourceExecutor = executors->GetResourceExecutor();
    if (!resourceExecutor) return;

    {
        auto id = resourceExecutor->AddT<CreateBufferParams>(
                CreateBufferParams{ BufferDesc{
                    vertices.size() * sizeof(MeshVertex),
                    BufferUsage::VERTEX,
                    MemoryType::DEVICE_LOCAL,
                    static_cast<uint32_t>(sizeof(MeshVertex)),
                    false,
                    pathName + "_VBO" } })
            .RemoveAfterExecute()
            .MarkProcess()
            .Commit();
        RHI_ONCE(resourceExecutor, id, CreateBufferResult, r, { vboHandle = r.handle; });
        resourceExecutor->ProcessParams(*device);

        if (vboHandle.id != 0) {
            std::span<const std::byte> bytes = std::as_bytes(std::span{vertices});
            std::vector<std::byte> data(bytes.begin(), bytes.end());

            resourceExecutor->AddT(
                    UpdateBufferParams{ vboHandle, 0, std::move(data) })
                .RemoveAfterExecute()
                .MarkProcess()
                .Commit();
            resourceExecutor->ProcessParams(*device);
        }
    }

    {
        auto id = resourceExecutor->AddT<CreateBufferParams>(
                CreateBufferParams{ BufferDesc{
                    indices.size() * sizeof(unsigned int),
                    BufferUsage::INDEX,
                    MemoryType::DEVICE_LOCAL,
                    0,
                    false,
                    pathName + "_IBO" } })
            .RemoveAfterExecute()
            .MarkProcess()
            .Commit();
        RHI_ONCE(resourceExecutor, id, CreateBufferResult, r, { iboHandle = r.handle; });
        resourceExecutor->ProcessParams(*device);

        if (iboHandle.id != 0) {
            std::span<const std::byte> bytes = std::as_bytes(std::span{indices});
            std::vector<std::byte> data(bytes.begin(), bytes.end());

            resourceExecutor->AddT<UpdateBufferParams>(
                    UpdateBufferParams{ iboHandle, 0, std::move(data) })
                .RemoveAfterExecute()
                .Commit();
            resourceExecutor->ProcessParams(*device);
        }
    }
}

void Mesh::DeleteBuffers() {
    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* device = rhi.Device();
    auto* executorsBase = rhi.Executors();
    auto* executors = static_cast<ExecutorsOwnerT<Backend>*>(executorsBase);

    auto* resourceExecutor = executors->GetResourceExecutor();
    if (!resourceExecutor) return;

    if (vboHandle.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(
                DestroyBufferParams{ vboHandle })
            .RemoveAfterExecute()
            .MarkProcess()
            .Commit();
        vboHandle = {};
    }
    if (iboHandle.id != 0) {
        resourceExecutor->AddT<DestroyBufferParams>(
                DestroyBufferParams{ iboHandle })
            .RemoveAfterExecute()
            .MarkProcess()
            .Commit();
        iboHandle = {};
    }
    resourceExecutor->ProcessParams(*device);
}

bool Mesh::Load() {
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
    CreateBuffers();

    return true;
}

void Mesh::Unload() {
    DeleteBuffers();

    vertices.clear();
    indices.clear();
    scene = nullptr;
    importer.FreeScene();
}