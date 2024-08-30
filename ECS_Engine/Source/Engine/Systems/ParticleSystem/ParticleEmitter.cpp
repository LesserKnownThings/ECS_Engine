#include "ParticleEmitter.h"
#include "glew.h"
#include "ParticleEmitterShapeManager.h"
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
		uint32_t buffers[] = { ebo, vbo, perIndexVbo, ssbo, initialSsbo };

		glDeleteBuffers(5, buffers);
		glDeleteVertexArrays(1, &vao);
	}

	ParticleEmitter::ParticleEmitter(EParticleSimType type)
	{
		commonParticleData.simType = type;

		//Shape type is hardcoded for now since 1 I don't have a system to serialize the objects yet and 2 I don't have a world to manage objects so I can't exactly set them up from UI
		ParticleEmitterShapeManager::Get().GetEmitterTypeData("Sphere", particleShapeData);

		switch (commonParticleData.simType)
		{
		case CPU:
			CPUInitialize();
			break;
		default:
			GPUInitialize();
			break;
		}		
	}

	void ParticleEmitter::SetSimType(EParticleSimType type)
	{
		commonParticleData.simType = type;
	}

	void ParticleEmitter::GPUInitialize()
	{
		CreateComputerBuffers();
		SetupGPUVertexBuffers();
	}

	void ParticleEmitter::CPUInitialize()
	{
		AllocateMemory(commonParticleData.maxAmount);
		SetupCPUVertexBuffers();
	}

	void ParticleEmitter::CreateComputerBuffers()
	{
		CreateGPUParticles(
			[this](ParticleData* pd, ParticleInitialData* pid)
			{
				glGenBuffers(1, &ssbo);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleData) * commonParticleData.maxAmount, pd, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

				//Creating 2 buffers one for initial data and another for current
				glGenBuffers(1, &initialSsbo);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, initialSsbo);
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleInitialData) * commonParticleData.maxAmount, pid, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, initialSsbo);
			});
	}

	void ParticleEmitter::CreateGPUParticles(std::function<void(ParticleData* pd, ParticleInitialData* pid)> func)
	{
		ParticleData* particleData = new ParticleData[commonParticleData.maxAmount];
		ParticleInitialData* particleInitialData = new ParticleInitialData[commonParticleData.maxAmount];

		for (int32 i = 0; i < commonParticleData.maxAmount; ++i)
		{
			const int32 iterator = i;

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
		//We don't want to overflow our buffer
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

		temp.color = (glm::vec4*)(temp.buffer);
		temp.position = (glm::vec3*)(size + temp.color);
		temp.velocity = size + temp.position;
		temp.scale = size + temp.velocity;
		temp.lifetime = (float*)(size + temp.scale);

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		MeshLoadingSystem::Get().ImportMesh("Data/quad.obj");

		MeshLoadingSystem::Get().GetMeshData("Data/quad.obj", [this](const DrawData& tempData)
			{
				elementsCount = tempData.indicesCount;

				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glGenBuffers(1, &ebo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				const int32 elementBufferSize = tempData.indicesCount * sizeof(uint32_t);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize, tempData.indices.data(), GL_STATIC_DRAW);

				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				const int32 vertexBufferSize = tempData.vertexCount * sizeof(VertexData);
				glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, tempData.vertexData.buffer, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, perIndexVbo);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(glm::vec4));
				glVertexAttribDivisor(1, 1);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			});

		glBindVertexArray(0);
	}

	void ParticleEmitter::SetupGPUVertexBuffers()
	{
		MeshLoadingSystem::Get().ImportMesh("Data/quad.obj");

		MeshLoadingSystem::Get().GetMeshData("Data/quad.obj", [this](const DrawData& tempData)
			{
				elementsCount = tempData.indicesCount;

				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glGenBuffers(1, &ebo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				const int32 elementBufferSize = tempData.indicesCount * sizeof(uint32_t);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize, tempData.indices.data(), GL_STATIC_DRAW);

				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				const int32 vertexBufferSize = tempData.vertexCount * sizeof(VertexData);
				glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, tempData.vertexData.buffer, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			});

		glBindVertexArray(0);
	}

	void ParticleEmitter::Simulate(float deltaTime)
	{
		switch (commonParticleData.simType)
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

		while (spawnAccumulator >= spawnInterval) {
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
		ShaderManager::Get().SetMat4f("model", model);
		glBindVertexArray(vao);

		switch (commonParticleData.simType)
		{
		case CPU:
			RenderCPU();
			break;
		default:
			RenderGPU();
			break;
		}

		glBindVertexArray(0);
	}

	void ParticleEmitter::RenderGPU()
	{
		glDrawElementsInstanced(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0, commonParticleData.maxAmount);
	}

	void ParticleEmitter::RenderCPU()
	{
		glBindBuffer(GL_ARRAY_BUFFER, perIndexVbo);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4), sizeof(glm::vec3) * particleDataCPU.instancesCount, particleDataCPU.position);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawElementsInstanced(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0, particleDataCPU.instancesCount);
	}
}