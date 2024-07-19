#include "MeshLoadingSystem.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <iostream>

MeshLoadingSystem::~MeshLoadingSystem()
{
}

MeshLoadingSystem::MeshLoadingSystem()
{
}

MeshLoadingSystem& MeshLoadingSystem::Get()
{
	static MeshLoadingSystem instance;

	return instance;
}

bool MeshLoadingSystem::GetMeshData(const std::string& meshPath, std::function<void(const DrawData& tempData)> func) const
{
	auto it = meshes.find(meshPath);

	if (it != meshes.end())
	{
		func(it->second);
		return true;
	}

	return false;
}

void MeshLoadingSystem::ImportMesh(const std::string& meshPath)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(meshPath,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_DropNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

    ProcessNode(scene->mRootNode, scene, meshPath);
}

void MeshLoadingSystem::ProcessNode(aiNode* node, const aiScene* scene, const std::string& path)
{
	//TODO support multiple meshes import

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace(path, ProcessMesh(mesh, scene, path));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, path);
	}
}

DrawData MeshLoadingSystem::ProcessMesh(aiMesh* assimp_mesh, const aiScene* scene, const std::string& path)
{
    DrawData drawData;
    drawData.vertexCount = assimp_mesh->mNumVertices;

    std::vector<uint32> indices;

    const int32 memoryToAllocate = assimp_mesh->mNumVertices * (sizeof(Vector3) * 2 + sizeof(Vector2));
    drawData.vertexData.buffer = malloc(memoryToAllocate);

    drawData.vertexData.position = (Vector3*)(drawData.vertexData.buffer);
    drawData.vertexData.normal = assimp_mesh->mNumVertices + drawData.vertexData.position;
    drawData.vertexData.uv = (Vector2*)(assimp_mesh->mNumVertices + drawData.vertexData.normal);

    for (int32 i = 0; i < assimp_mesh->mNumVertices; i++)
    {
        Vector3 vector;

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
            Vector2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = assimp_mesh->mTextureCoords[0][i].x;
            vec.y = assimp_mesh->mTextureCoords[0][i].y;
            drawData.vertexData.uv[i] = vec;

            //TODO implement tangent and bitangent in VertexData
            //// tangent
            //vector.x = assimp_mesh->mTangents[i].x;
            //vector.y = assimp_mesh->mTangents[i].y;
            //vector.z = assimp_mesh->mTangents[i].z;
            //vertexData.tangent = vector;
            //// bitangent
            //vector.x = assimp_mesh->mBitangents[i].x;
            //vector.y = assimp_mesh->mBitangents[i].y;
            //vector.z = assimp_mesh->mBitangents[i].z;
            //vertexData.bitangent = vector;
        }
        else
            drawData.vertexData.uv[i] = Vector2(0.0f, 0.0f);
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

    //TODO implement a material system for rendering, for now we can render without one, or hardcode a texture
    aiMaterial* material = scene->mMaterials[assimp_mesh->mMaterialIndex];

    drawData.indicesCount = indices.size();
    drawData.indices = std::move(indices);

    return drawData;
}