#include "ParticleEmitter.h"
#include "Assets/Mesh.h"
#include "glm/gtc/type_ptr.hpp"
#include "glew/glew.h"
#include "ParticleSystem.h"
#include "ParticleEmitterShapeManager.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/MeshLoadingSystem.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/ShaderSystem/ShaderProgram.h"

#include <iostream>

namespace LKT
{
	namespace
	{
		static uint32_t GetCPUParticleDataSize()
		{
			return sizeof(glm::vec4) + sizeof(glm::vec3) * 3 + sizeof(float);
		}
	}

	ParticleEmitter::~ParticleEmitter()
	{
		uint32_t buffers[] = {perIndexVbo, ssbo, initialSsbo};

		glDeleteBuffers(3, buffers);

		free(particleDataCPU.buffer);
	}

	bool ParticleEmitter::Serialize(std::ofstream &outStream) const
	{
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.initialColor), sizeof(Color));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.rate), sizeof(uint32_t));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.maxAmount), sizeof(uint32_t));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.speed), sizeof(float));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.lifetime), sizeof(float));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.useGravity), sizeof(bool));
		outStream.write(reinterpret_cast<const char *>(&commonParticleData.isLocal), sizeof(bool));

		return true;
	}

	bool ParticleEmitter::Deserialize(std::ifstream &inStream)
	{
		inStream.read(reinterpret_cast<char *>(&commonParticleData.initialColor), sizeof(Color));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.rate), sizeof(uint32_t));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.maxAmount), sizeof(uint32_t));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.speed), sizeof(float));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.lifetime), sizeof(float));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.useGravity), sizeof(bool));
		inStream.read(reinterpret_cast<char *>(&commonParticleData.isLocal), sizeof(bool));

		return true;
	}

	void ParticleEmitter::SerializeInitialData(std::ofstream &stream)
	{
		const uint32_t defaultRate = 10;
		const uint32_t defaultInitialAmount = 1500;
		const float defaultLifetime = 5.0f;
		const bool defaultIsLocal = false;

		stream.write(reinterpret_cast<const char *>(&Color::white), sizeof(Color));
		stream.write(reinterpret_cast<const char *>(&defaultRate), sizeof(uint32_t));
		stream.write(reinterpret_cast<const char *>(&defaultInitialAmount), sizeof(uint32_t));
		stream.write(reinterpret_cast<const char *>(&defaultLifetime), sizeof(float));
		stream.write(reinterpret_cast<const char *>(&defaultLifetime), sizeof(float));
		stream.write(reinterpret_cast<const char *>(&defaultIsLocal), sizeof(bool));
		stream.write(reinterpret_cast<const char *>(&defaultIsLocal), sizeof(bool));
	}

	ParticleEmitter::ParticleEmitter(ParticleSystem *inParent)
		: parent(inParent)
	{
		auto func = [this](const LazyAssetPtr<Asset> &asset)
		{
			particleMesh = std::move(asset.StrongRef());
		};

		AssetPath path{"Data/Content/Assets/Plane.asset"};
		AssetManager::GetAsset(path, func);

		// Shape type is hardcoded for now since 1 I don't have a system to serialize the objects yet and 2 I don't have a world to manage objects so I can't exactly set them up from UI
		ParticleEmitterShapeManager::Get().GetEmitterTypeData("Sphere", particleShapeData);

		switch (parent->GetSimType())
		{
		case CPU:
			CPUInitialize();
			break;
		default:
			GPUInitialize();
			break;
		}
	}

	void ParticleEmitter::GPUInitialize()
	{
		CreateComputeBuffers();
	}

	void ParticleEmitter::CPUInitialize()
	{
		AllocateMemory(commonParticleData.maxAmount);
		SetupCPUVertexBuffers();
	}

	void ParticleEmitter::CreateComputeBuffers()
	{
		CreateGPUParticles(
			[this](ParticleData *pd, ParticleInitialData *pid)
			{
				glGenBuffers(1, &ssbo);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleData) * commonParticleData.maxAmount, pd, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

				// Creating 2 buffers one for initial data and another for current
				glGenBuffers(1, &initialSsbo);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, initialSsbo);
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleInitialData) * commonParticleData.maxAmount, pid, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, initialSsbo);
			});
	}

	void ParticleEmitter::CreateGPUParticles(std::function<void(ParticleData *pd, ParticleInitialData *pid)> func)
	{
		ParticleData *particleData = new ParticleData[commonParticleData.maxAmount];
		ParticleInitialData *particleInitialData = new ParticleInitialData[commonParticleData.maxAmount];

		for (int32_t i = 0; i < commonParticleData.maxAmount; ++i)
		{
			const int32_t iterator = i;

			glm::vec3 pos, vel;
			if (ParticleEmitterShapeManager::Get().GetShapeData(particleShapeData, pos, vel))
			{
				particleData[iterator].positionLifetime = glm::vec4(pos.x, pos.y, pos.z, commonParticleData.lifetime);
				particleInitialData[iterator].positionLifetime = particleData[iterator].positionLifetime;

				glm::vec3 normalizedDir = vel * commonParticleData.speed;
				particleData[iterator].velocityAlpha = glm::vec4(normalizedDir.x, normalizedDir.y, normalizedDir.z, commonParticleData.initialColor.a);
			}

			particleData[i].colorUseGravity = glm::vec4(commonParticleData.initialColor.r, commonParticleData.initialColor.g, commonParticleData.initialColor.b, false);

			float spawnTime = static_cast<float>(i) / (static_cast<float>(commonParticleData.rate));

			particleData[i].scaleSpawnTime = glm::vec4(1.f, 1.f, 1.f, spawnTime);
			particleInitialData[i].scaleSpawnTime = glm::vec4(1.f, 1.f, 1.f, spawnTime);

			particleData[i].worldModel = glm::mat4(1.0f);
			particleData[i].isLocal = commonParticleData.isLocal;
			particleData[i].isActive = false;
		}

		func(particleData, particleInitialData);

		delete[] particleData;
		delete[] particleInitialData;
	}

	void ParticleEmitter::CreateCPUParticle(int32_t index)
	{
		// We don't want to overflow our buffer
		if (particleDataCPU.instancesCount == particleDataCPU.allocatedCount)
		{
			return;
		}

		particleDataCPU.color[index] = commonParticleData.initialColor.GetVectorAlpha();
		particleDataCPU.lifetime[index] = commonParticleData.lifetime;

		particleDataCPU.instancesCount++;

		ParticleEmitterShapeManager::Get().GetShapeData(particleShapeData, particleDataCPU.position[index], particleDataCPU.velocity[index]);

		glBindBuffer(GL_ARRAY_BUFFER, perIndexVbo);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * (index + 1), sizeof(glm::vec3), glm::value_ptr(particleDataCPU.position[index]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void ParticleEmitter::SwapParticleData(int32_t index)
	{
		int32_t swappingIndex = particleDataCPU.instancesCount - 1;

		std::swap(particleDataCPU.position[index], particleDataCPU.position[swappingIndex]);
		std::swap(particleDataCPU.lifetime[index], particleDataCPU.lifetime[swappingIndex]);
		particleDataCPU.instancesCount--;
	}

	void ParticleEmitter::AllocateMemory(int32_t size)
	{
		ParticleDataCPU temp;
		temp.instancesCount = particleDataCPU.instancesCount;
		temp.allocatedCount = size;

		int32_t allocation = size * GetCPUParticleDataSize();
		temp.buffer = malloc(allocation);

		temp.color = (glm::vec4 *)(temp.buffer);
		temp.position = (glm::vec3 *)(size + temp.color);
		temp.velocity = size + temp.position;
		temp.scale = size + temp.velocity;
		temp.lifetime = (float *)(size + temp.scale);

		memcpy(temp.color, particleDataCPU.color, temp.instancesCount * sizeof(glm::vec4));
		memcpy(temp.position, particleDataCPU.position, temp.instancesCount * sizeof(glm::vec3));
		memcpy(temp.velocity, particleDataCPU.velocity, temp.instancesCount * sizeof(glm::vec3));
		memcpy(temp.scale, particleDataCPU.scale, temp.instancesCount * sizeof(glm::vec3));
		memcpy(temp.lifetime, particleDataCPU.position, temp.instancesCount * sizeof(float));

		free(particleDataCPU.buffer);
		particleDataCPU = temp;
	}

	void ParticleEmitter::SetupCPUVertexBuffers()
	{
		glGenBuffers(1, &perIndexVbo);
		glBindBuffer(GL_ARRAY_BUFFER, perIndexVbo);
		const int32_t size = commonParticleData.maxAmount * GetCPUParticleDataSize();
		glBufferData(GL_ARRAY_BUFFER, size, particleDataCPU.buffer, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)sizeof(glm::vec4));
		glVertexAttribDivisor(1, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void ParticleEmitter::Simulate(float deltaTime)
	{
		switch (parent->GetSimType())
		{
		case CPU:
			SimulateCPU(deltaTime);
			break;
		default:
			SimulateGPU(deltaTime);
			break;
		}
	}

	void ParticleEmitter::SimulateGPU(float deltaTime)
	{
		ShaderManager::Get().SetFloat("deltaTime", deltaTime);
		ShaderManager::Get().SetMat4f("worldModel", model);

		const uint32_t xSize = (commonParticleData.maxAmount + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;

		glDispatchCompute(xSize, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void ParticleEmitter::SimulateCPU(float deltaTime)
	{
		const float spawnInterval = 1.0f / static_cast<float>(commonParticleData.rate);
		spawnAccumulator += deltaTime;

		while (spawnAccumulator >= spawnInterval)
		{
			CreateCPUParticle(particleDataCPU.instancesCount);
			spawnAccumulator -= spawnInterval;
		}

#pragma omp parallel for
		for (int32_t i = 0; i < particleDataCPU.instancesCount; ++i)
		{
			particleDataCPU.lifetime[i] -= deltaTime;

			if (particleDataCPU.lifetime[i] <= 0)
			{
				SwapParticleData(i);
			}
			else
			{
				particleDataCPU.position[i] += commonParticleData.speed * particleDataCPU.velocity[i] * deltaTime;
			}
		}
	}

	void ParticleEmitter::Render()
	{
		//ShaderManager::Get().SetMat4f("model", model);

		if (const Mesh *mesh = particleMesh.Get<Mesh>())
		{
			const RenderComponentObject &rco = mesh->GetRenderComponentObject();
			glBindVertexArray(rco.vao);

			switch (parent->GetSimType())
			{
			case CPU:
				RenderCPU();
				break;
			}

			glDrawElementsInstanced(GL_TRIANGLES, rco.elementCount, GL_UNSIGNED_INT, 0, particleDataCPU.instancesCount);
		}
		glBindVertexArray(0);
	}

	void ParticleEmitter::RenderCPU()
	{
		glBindBuffer(GL_ARRAY_BUFFER, perIndexVbo);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4), sizeof(glm::vec3) * particleDataCPU.instancesCount, particleDataCPU.position);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}