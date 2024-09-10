#include "MeshLoadingSystem.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <filesystem>
#include <iostream>

namespace LKT
{
    MeshLoadingSystem &MeshLoadingSystem::Get()
    {
        static MeshLoadingSystem instance;

        return instance;
    }

    bool MeshLoadingSystem::ImportMesh(const std::string &meshPath, std::vector<MeshData> &outMeshData)
    {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(meshPath,
                                               aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_DropNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return false;
        }

        MeshLoadingSystem::Get().ProcessNode(scene->mRootNode, scene, meshPath, outMeshData);
        return true;
    }

    void MeshLoadingSystem::ProcessNode(aiNode *node, const aiScene *scene, const std::string &path, std::vector<MeshData> &outMeshData)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            outMeshData.push_back(std::move(ProcessMesh(mesh, scene, path)));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, path, outMeshData);
        }
    }

    // TODO add a move to DrawData instead of copy
    MeshData MeshLoadingSystem::ProcessMesh(aiMesh *assimp_mesh, const aiScene *scene, const std::string &path)
    {
        MeshData drawData;
        drawData.vertexCount = assimp_mesh->mNumVertices;
        drawData.name = assimp_mesh->mName.C_Str();
        drawData.nameSize = drawData.name.size();

        std::vector<uint32_t> indices;

        const int32_t memoryToAllocate = assimp_mesh->mNumVertices * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2));
        drawData.vertexData.buffer = malloc(memoryToAllocate);

        drawData.vertexData.position = (glm::vec3 *)(drawData.vertexData.buffer);
        drawData.vertexData.normal = assimp_mesh->mNumVertices + drawData.vertexData.position;
        drawData.vertexData.uv = (glm::vec2 *)(assimp_mesh->mNumVertices + drawData.vertexData.normal);

        for (int32_t i = 0; i < assimp_mesh->mNumVertices; i++)
        {
            glm::vec3 vector;

            vector.x = assimp_mesh->mVertices[i].x;
            vector.y = assimp_mesh->mVertices[i].y;
            vector.z = assimp_mesh->mVertices[i].z;
            drawData.vertexData.position[i] = vector;

            if (assimp_mesh->HasNormals())
            {
                vector.x = assimp_mesh->mNormals[i].x;
                vector.y = assimp_mesh->mNormals[i].y;
                vector.z = assimp_mesh->mNormals[i].z;
                drawData.vertexData.normal[i] = vector;
            }

            if (assimp_mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = assimp_mesh->mTextureCoords[0][i].x;
                vec.y = assimp_mesh->mTextureCoords[0][i].y;
                drawData.vertexData.uv[i] = vec;

                // TODO implement tangent and bitangent in VertexData
                //// tangent
                // vector.x = assimp_mesh->mTangents[i].x;
                // vector.y = assimp_mesh->mTangents[i].y;
                // vector.z = assimp_mesh->mTangents[i].z;
                // vertexData.tangent = vector;
                //// bitangent
                // vector.x = assimp_mesh->mBitangents[i].x;
                // vector.y = assimp_mesh->mBitangents[i].y;
                // vector.z = assimp_mesh->mBitangents[i].z;
                // vertexData.bitangent = vector;
            }
            else
                drawData.vertexData.uv[i] = glm::vec2(0.0f, 0.0f);
        }

        uint32_t offset = 0;
        for (unsigned int i = 0; i < assimp_mesh->mNumFaces; i++)
        {
            aiFace face = assimp_mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // TODO implement a material system for rendering, for now we can render without one, or hardcode a texture
        aiMaterial *material = scene->mMaterials[assimp_mesh->mMaterialIndex];

        // aiString texturePath;
        // if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
        // {
        //     drawData.texturePath = defaultTexturePath;
        // }
        // else
        // {
        //     drawData.texturePath = texturePath.C_Str();
        // }

        drawData.indicesCount = indices.size();
        drawData.indices = std::move(indices);

        return drawData;
    }

    VertexData::~VertexData()
    {
        free(buffer);
        buffer = nullptr;
    }

    void MeshData::Serialize(std::ofstream &outStream) const
    {
        outStream.write(reinterpret_cast<const char *>(&vertexCount), sizeof(int32_t));
        outStream.write(reinterpret_cast<const char *>(&indicesCount), sizeof(int32_t));

        const int32_t bufferSize = vertexCount * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2));
        outStream.write(reinterpret_cast<const char *>(vertexData.buffer), bufferSize);

        const int32_t indicesBufferSize = indicesCount * sizeof(uint32_t);
        if (indicesBufferSize > 0)
        {
            outStream.write(reinterpret_cast<const char *>(indices.data()), indicesBufferSize);
        }

        outStream.write(reinterpret_cast<const char *>(&nameSize), sizeof(int32_t));
        outStream.write(reinterpret_cast<const char *>(&name), nameSize);
    }

    void MeshData::Deserialize(std::ifstream &inStream)
    {
        inStream.read(reinterpret_cast<char *>(&vertexCount), sizeof(int32_t));
        inStream.read(reinterpret_cast<char *>(&indicesCount), sizeof(int32_t));

        const int32_t bufferSize = vertexCount * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2));
        inStream.read(reinterpret_cast<char *>(vertexData.buffer), bufferSize);

        const int32_t indicesBufferSize = indicesCount * sizeof(uint32_t);

        if (indicesBufferSize > 0)
        {
            indices.resize(indicesCount);
            inStream.read(reinterpret_cast<char *>(&indices[0]), indicesBufferSize);
        }

        inStream.read(reinterpret_cast<char *>(&nameSize), sizeof(int32_t));

        name.resize(nameSize);
        inStream.read(reinterpret_cast<char *>(&name[0]), nameSize);
    }

    void MeshData::ReadBuffer(void *buffer)
    {
        char *bufferMod = static_cast<char *>(buffer);

        memcpy(&vertexCount, bufferMod, sizeof(int32_t));

        int32_t offset = sizeof(int32_t);
        memcpy(&indicesCount, bufferMod + offset, sizeof(int32_t));
        offset += sizeof(int32_t);

        const int32_t bufferSize = vertexCount * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2));
        vertexData.buffer = malloc(bufferSize);
        memcpy(vertexData.buffer, bufferMod + offset, bufferSize);
        offset += bufferSize;

        const int32_t indicesBufferSize = indicesCount * sizeof(uint32_t);
        if (indicesBufferSize > 0)
        {
            indices.resize(indicesCount);
            memcpy(&indices[0], bufferMod + offset, indicesBufferSize);
        }
        offset += indicesBufferSize;

        memcpy(&nameSize, bufferMod + offset, sizeof(int32_t));
        offset += sizeof(int32_t);

        name.resize(nameSize);
        memcpy(&name[0], bufferMod + offset, nameSize);
    }
}