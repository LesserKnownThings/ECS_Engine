#pragma once

#include "Color.h"
#include "glm/glm.hpp"
#include "Object.h"
#include "ParticleShapeData.h"
#include "ParticleSimType.h"
#include "Systems/AssetManager/ISerializable.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace LKT
{
	class Asset;
	class ParticleSystem;
	class ParticleEmitterShape;
	class ShaderProgram;

	constexpr uint32_t WORK_GROUP_SIZE = 256;

	struct alignas(16) ParticleData
	{
		glm::vec4 positionLifetime;
		glm::vec4 velocityAlpha;
		glm::vec4 colorUseGravity;
		glm::vec4 scaleSpawnTime;
		glm::mat4 worldModel;
		uint32_t isLocal;
		uint32_t isActive;
	};

	struct alignas(16) ParticleInitialData
	{
		glm::vec4 positionLifetime;
		glm::vec4 scaleSpawnTime;
	};

	struct CommonParticleData
	{
		Color initialColor = Color::white;

		uint32_t rate = 10;
		uint32_t maxAmount = 10500;

		float speed = 5.0f;
		float lifetime = 5.0f;

		bool useGravity = false;
		bool isLocal = false;
	};

	struct ParticleDataCPU
	{
		int32_t instancesCount = 0;
		int32_t allocatedCount = 0;

		void *buffer = nullptr;

		glm::vec4 *color = nullptr;
		glm::vec3 *position = nullptr;
		glm::vec3 *velocity = nullptr;
		glm::vec3 *scale = nullptr;
		float *lifetime = nullptr;
	};

	class ParticleEmitter : public Object, public ISerializable
	{
	public:
		~ParticleEmitter();

	protected:
		bool Serialize(std::ofstream &outStream) const override;
		bool Deserialize(std::ifstream &inStream) override;

		static void SerializeInitialData(std::ofstream &stream);

	private:
		ParticleEmitter(ParticleSystem *inParent);

		void Simulate(float deltaTime);
		void SimulateGPU(float deltaTime);
		void SimulateCPU(float deltaTime);

		void Render();
		void RenderCPU();

		void CreateGPUParticles(std::function<void(ParticleData *pd, ParticleInitialData *pid)> func);
		void CreateCPUParticle(int32_t index);
		void SwapParticleData(int32_t index);

		void GPUInitialize();
		void CPUInitialize();

		void CreateComputeBuffers();

		void SetupCPUVertexBuffers();

		void AllocateMemory(int32_t size);

		CommonParticleData commonParticleData;
		float spawnAccumulator = 0.0f;
		ParticleShapeData particleShapeData;

		ParticleDataCPU particleDataCPU;

		uint32_t elementsCount;
		uint32_t perIndexVbo, ssbo, initialSsbo;
		glm::mat4 model = glm::mat4(1.0f);

		ParticleSystem *parent = nullptr;
		LazyAssetPtr<Asset> particleMesh;

		friend class ParticleSystem;
		friend class ParticleSystemAssetViewer;
	};
}