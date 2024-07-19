#pragma once

#include "../Math//Numerals.h"
#include "../Math//Transforms.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct aiMesh;
struct aiNode;
struct aiScene;

/// <summary>
/// The system that will manage loading all the meshes in the engine
/// This systems uses assimp for the import
/// </summary>

struct VertexData
{
	void* buffer;

	Vector3* position;
	Vector3* normal;
	Vector2* uv;	
};

struct DrawData
{
	DrawData() = default;

	int32 vertexCount;
	int32 indicesCount;	

	VertexData vertexData;
	std::vector<uint32> indices;
};

class MeshLoadingSystem
{

public:
	~MeshLoadingSystem();
	MeshLoadingSystem();

	static MeshLoadingSystem& Get();

	void ImportMesh(const std::string& meshPath);
	bool GetMeshData(const std::string& meshPath, std::function<void(const DrawData& tempData)> func) const;

private:
	void ProcessNode(aiNode* node, const aiScene* scene, const std::string& path);
	DrawData ProcessMesh(aiMesh* assimpMesh, const aiScene* scene, const std::string& path);

	std::unordered_map<std::string, DrawData> meshes;
};