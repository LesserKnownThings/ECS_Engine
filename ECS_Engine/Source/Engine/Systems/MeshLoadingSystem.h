#pragma once

#include "glm/glm.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <fstream>
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
		~VertexData();

		VertexData() = default;
		VertexData(const VertexData &other)
			: buffer(other.buffer)
		{
		}

		VertexData(VertexData &&other)
			: buffer(std::move(other.buffer))
		{
			other.buffer = nullptr;
		}

		VertexData &operator=(const VertexData &other)
		{
			if (&other != this)
			{
				buffer = other.buffer;
			}
			return *this;
		}

		void *buffer;

		glm::vec3 *position;
		glm::vec3 *normal;
		glm::vec2 *uv;
	};

	struct MeshData
	{
		MeshData() = default;

		void Serialize(std::ofstream &outStream) const;
		void Deserialize(std::ifstream &inStream);

		void ReadBuffer(void *buffer);

		bool IsValid() const { return vertexCount > 0; }

		int32_t vertexCount;
		int32_t indicesCount;

		VertexData vertexData;
		std::vector<uint32_t> indices;

		int32_t nameSize;
		std::string name;
	};

	class MeshLoadingSystem
	{
	public:
		~MeshLoadingSystem() = default;

		static MeshLoadingSystem &Get();

		static bool ImportMesh(const std::string &meshPath, std::vector<MeshData> &outMeshData);

	private:
		MeshLoadingSystem() = default;

		void ProcessNode(aiNode *node, const aiScene *scene, const std::string &path, std::vector<MeshData> &outMeshData);
		MeshData ProcessMesh(aiMesh *assimpMesh, const aiScene *scene, const std::string &path);
	};
}