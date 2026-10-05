#include "Renderer/3D/Mesh/Model.h"

#include <iostream>

#include <cerrno>
#include <cstring>

#include "RHI/Fabric/ExecutorsOwnerT.h"
#include "RHI/RuntimeRHI.h"
#include "RHI/Base/Executors/ExecutorTypeRHI.h"

#include "RHI/Base/Models/BufferExecutorParams.h"
#include "RHI/Base/Models/BufferExecutorResult.h"
#include "RHI/Fabric/BackendTraits.h"

#include "RHI/RuntimeRHI.h"

using namespace RHI;
using namespace RHI::Executors;
using namespace RHI::Executors::BufferExecute;
using namespace RHI::FabricRHI;

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh* mesh) {
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

void Model::CreateBuffers() {
    auto& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* device = rhi.Device();
    auto* iexecuters = rhi.Executors();
    auto* executors = static_cast<ExecutorsOwnerT<CurrentBackend>*>(iexecuters);

    auto* bufferExec = executors->GetExecutorAs<CurrentBackend::BufferExecutor>(ExecutorTypeRHI::BufferExecutor);

    if (!bufferExec) return;

    auto submit = [](CurrentBackend::BufferExecutor* exec, auto param, int layer = 0) {
        return exec->AddParamAndMark(param, layer, true);
    };

    {
        VBOCreateParams vboParams(vertices.data(),
                                  vertices.size() * sizeof(Vertex),
                                  BufferUsage::Static, true, true);
        const uint64_t vboParamId = submit(bufferExec, BufferParams{vboParams});

        bufferExec->SubscribeOnce(vboParamId,
            [this](const std::vector<BufferExecuteResult>& results) {
                if (results.empty()) return;
                if (auto* vboResult = std::get_if<VBOCreateResult>(&results[0].data)) {
                    this->VBO = vboResult->vboID;
                }
            });

        bufferExec->ProcessParams(*device);
    }

    {
        IBOCreateParams iboParams(indices.data(),
                                  indices.size(),
                                  IndexType::UInt,
                                  BufferUsage::Static, true, true);
        const uint64_t iboParamId = submit(bufferExec, BufferParams{iboParams});

        bufferExec->SubscribeOnce(iboParamId,
            [this](const std::vector<BufferExecuteResult>& results) {
                if (results.empty()) return;
                if (auto* iboResult = std::get_if<IBOCreateResult>(&results[0].data)) {
                    this->IBO = iboResult->iboID;
                }
            });

        bufferExec->ProcessParams(*device);
    }

    {
        VAOCreateParams vaoParams(VBO, IBO, true, true);
        const uint64_t vaoParamId = submit(bufferExec, BufferParams{vaoParams});

        bufferExec->SubscribeOnce(vaoParamId,
            [this](const std::vector<BufferExecuteResult>& results) {
                if (results.empty()) return;
                if (auto* vaoResult = std::get_if<VAOCreateResult>(&results[0].data)) {
                    this->VAO = vaoResult->vaoID;
                }
            });

        bufferExec->ProcessParams(*device);
    }

    constexpr size_t stride = sizeof(Vertex);

    {
        VAOSetAttributeParams positionAttrParams(VAO, VBO, 0, AttributeType::Float3,
                                                 offsetof(Vertex, Position), stride,
                                                 false, false, true, 0);
        submit(bufferExec, BufferParams{positionAttrParams});
    }

    {
        VAOSetAttributeParams normalAttrParams(VAO, VBO, 1, AttributeType::Float3,
                                               offsetof(Vertex, Normal), stride,
                                               false, false, true, 0);
        submit(bufferExec, BufferParams{normalAttrParams});
    }

    {
        VAOSetAttributeParams texCoordAttrParams(VAO, VBO, 2, AttributeType::Float2,
                                                 offsetof(Vertex, TexCoords), stride,
                                                 false, false, true, 0);
        submit(bufferExec, BufferParams{texCoordAttrParams});
    }

    {
        VAOSetAttributeParams tangentAttrParams(VAO, VBO, 3, AttributeType::Float3,
                                                offsetof(Vertex, Tangent), stride,
                                                false, false, true, 0);
        submit(bufferExec, BufferParams{tangentAttrParams});
    }

    {
        VAOSetAttributeParams bitangentAttrParams(VAO, VBO, 4, AttributeType::Float3,
                                                  offsetof(Vertex, Bitangent), stride,
                                                  false, false, true, 0);
        submit(bufferExec, BufferParams{bitangentAttrParams});
    }

    bufferExec->ProcessParams(*device);
}

void Model::DeleteBuffers() {
    RuntimeRHI& rhi = RuntimeRHI::GetInstance();
    if (!rhi.IsInitialized()) return;

    auto* device = rhi.Device();
    auto* iexecuters = rhi.Executors();
    auto* executors = static_cast<ExecutorsOwnerT<CurrentBackend>*>(iexecuters);

    auto* bufferExec = executors->GetExecutorAs<CurrentBackend::BufferExecutor>(ExecutorTypeRHI::BufferExecutor);
    if (!bufferExec) return;

    if (VBO) {
        BufferDestroyParams p(VBO, BufferType::VBO);
        bufferExec->AddParamAndMark(BufferParams{p}, 0, true);
        VBO = 0;
    }
    if (IBO) {
        BufferDestroyParams p(IBO, BufferType::IBO);
        bufferExec->AddParamAndMark(BufferParams{p}, 0, true);
        IBO = 0;
    }
    if (VAO) {
        BufferDestroyParams p(VAO, BufferType::VAO);
        bufferExec->AddParamAndMark(BufferParams{p}, 0, true);
        VAO = 0;
    }

    bufferExec->ProcessParams(*device);
}

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
    CreateBuffers();

    return true;
}

void Model::Unload() {
    DeleteBuffers();

    vertices.clear();
    indices.clear();
    scene = nullptr;
    importer.FreeScene();
}