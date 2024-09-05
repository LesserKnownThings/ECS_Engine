#pragma once

#include "glm/glm.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct aiMesh;
struct aiNode;
struct aiScene;

namespace LKT
{
	/// <summary>
	/// The system that will manage loading all the meshes in the engine
	/// This systems uses assimp for the import
	/// </summary>

	struct VertexData
	{
		void* buffer;

		glm::vec3* position;
		glm::vec3* normal;
		glm::vec2* uv;
	};

	struct DrawData
	{
		DrawData() = default;

		int32_t vertexCount;
		int32_t indicesCount;

		VertexData vertexData;
		std::vector<uint32_t> indices;
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
}