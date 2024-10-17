#include "TransformSystem.h"
#include "EntityManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Random.h"
#include "ShaderSystem/ShaderManager.h"
#include "TaskManagerSystem.h"
#include "WorkDispatcherSystem.h"

#include <cassert>
#include <cstring>
#include <iostream>

namespace LKT
{
	TransformSystem::TransformSystem()
	{
		AllocateMemory(DEFAULT_ALLOCATED_MEMORY);

		TaskManagerSystem::Get().RegisterTask(this, &TransformSystem::Process, 0, PROCESS_HANDLE);
		TaskManagerSystem::Get().RegisterTask(this, &TransformSystem::GarbageCollectionPass, 0, GC_HANDLE);
	}

	TransformSystem::~TransformSystem()
	{
	}

	TransformSystem &TransformSystem::Get()
	{
		static TransformSystem instance;
		return instance;
	}

	void TransformSystem::Process(float deltaTime)
	{
		std::vector<Job> tasks;

		for (uint32_t i = 0; i < transformComponent.instancesCount; i += MAX_THREAD_WORKLOAD)
		{
			uint32_t endIndex = std::min(i + MAX_THREAD_WORKLOAD, (uint32_t)transformComponent.instancesCount);

			tasks.emplace_back([this, deltaTime, i, endIndex]()
							   { ProcessComponents(deltaTime, i, endIndex); });
		}

		WorkDispatcherSystem::Get().AddTasks(tasks);
	}

	void TransformSystem::GarbageCollectionPass()
	{
		const EntityManager &em = EntityManager::Get();
		uint32_t aliveInRow = 0;
		while (transformComponent.instancesCount > 0 && aliveInRow < 4)
		{
			uint32_t i = Random::RandomRange(0, transformComponent.instancesCount - 1);
			if (em.IsEntityAlive(transformComponent.entity[i]))
			{
				++aliveInRow;
				continue;
			}

			aliveInRow = 0;
			DestroyComponent(i);
		}
	}

	void TransformSystem::DestroyComponent(uint32_t c)
	{
		const uint32_t last = transformComponent.instancesCount - 1;
		const Entity currentEntity = transformComponent.entity[c];
		const Entity lastEntity = transformComponent.entity[last];

		transformComponent.entity[c] = transformComponent.entity[last];
		transformComponent.position[c] = transformComponent.position[last];
		transformComponent.eulerAngles[c] = transformComponent.eulerAngles[last];
		transformComponent.scale[c] = transformComponent.scale[last];

		instances[lastEntity] = c;
		instances.erase(currentEntity);
		transformComponent.instancesCount--;
	}

	void TransformSystem::ProcessComponents(float deltaTime, uint32_t startIndex, uint32_t endIndex)
	{
	}

	void TransformSystem::CreateComponents(int32_t entityCount,
										   Entity *entities,
										   const std::type_index &type,
										   void *commonData,
										   void *componentData)
	{
		const uint32_t startingIndex = instances.size();

		if (transformComponent.allocatedInstances < entityCount)
		{
			AllocateMemory(entityCount > startingIndex ? entityCount * 2 : startingIndex * 2);
		}

		transformComponent.instancesCount += entityCount;

		for (int32_t i = 0; i < entityCount; ++i)
		{
			instances.emplace(entities[i], startingIndex + i);
		}

		memcpy(&transformComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);

		memcpy(&transformComponent.position[startingIndex], componentData, sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.eulerAngles[startingIndex], ((char *)componentData + sizeof(glm::vec3) * entityCount), sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.scale[startingIndex], ((char *)componentData + sizeof(glm::vec3) * 2 * entityCount), sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);
	}

	bool TransformSystem::EntityHasComponent(const Entity &e) const
	{
		return instances.find(e.id) != instances.end();
	}

	bool TransformSystem::GetComponent(const Entity &e, uint32_t &outComponent) const
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			outComponent = it->second;
			return true;
		}

		return false;
	}

	bool TransformSystem::GetPosition(const Entity &e, glm::vec3 &outPos) const
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			outPos = transformComponent.position[it->second];
			return true;
		}

		return false;
	}

	void TransformSystem::SetPosition(const Entity &e, const glm::vec3 &position)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetPosition(it->second, position);
		}
	}

	void TransformSystem::SetRotation(const Entity &e, const glm::vec3 &eulerAngles)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetRotation(it->second, eulerAngles);
		}
	}

	void TransformSystem::SetScale(const Entity &e, const glm::vec3 &scale)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetScale(it->second, scale);
		}
	}

	void TransformSystem::SetPosition(uint32_t component, const glm::vec3 &position)
	{
		transformComponent.position[component] = position;
	}

	void TransformSystem::SetRotation(uint32_t component, const glm::vec3 &eulerAngles)
	{
		transformComponent.eulerAngles[component] = eulerAngles;
	}

	void TransformSystem::SetScale(uint32_t component, const glm::vec3 &scale)
	{
		transformComponent.scale[component] = scale;
	}

	void TransformSystem::Move(const Entity &e, const glm::vec3 &direction)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			Move(it->second, direction);
		}
	}

	void TransformSystem::Rotate(const Entity &e, const glm::vec3 &axis)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			Rotate(it->second, axis);
		}
	}

	void TransformSystem::Move(uint32_t component, const glm::vec3 &direction)
	{
		transformComponent.position[component] += direction;
	}

	void TransformSystem::Rotate(uint32_t component, const glm::vec3 &axis)
	{
		transformComponent.eulerAngles[component] += axis;
	}

	// TODO move the model to the component data, it will be better to set the model when the pos/rot/scale change instead of calculating a model every frame
	bool TransformSystem::GetModel(const Entity &e, glm::mat4 &outModel) const
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			outModel = glm::translate(outModel, transformComponent.position[it->second]);

			const float length = glm::length(transformComponent.eulerAngles[it->second]);
			if (length > 0)
			{
				const float x = transformComponent.eulerAngles[it->second].x;
				const float y = transformComponent.eulerAngles[it->second].y;
				const float z = transformComponent.eulerAngles[it->second].z;

				outModel = glm::rotate(outModel, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
				outModel = glm::rotate(outModel, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
				outModel = glm::rotate(outModel, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			outModel = glm::scale(outModel, transformComponent.scale[it->second]);

			return true;
		}

		return false;
	}

	void TransformSystem::AllocateMemory(int32_t size)
	{
		assert(("New allocated size needs to be bigger than old allocated size!", size > transformComponent.allocatedInstances));

		TransformComponent newComponent;

		const int32_t sizeToAllocate = size * (sizeof(glm::vec3) * 3 + sizeof(Entity));

		newComponent.buffer = malloc(sizeToAllocate);

		newComponent.instancesCount = transformComponent.instancesCount;
		newComponent.allocatedInstances = size;

		newComponent.position = (glm::vec3 *)(newComponent.buffer);
		newComponent.eulerAngles = newComponent.position + size;
		newComponent.scale = newComponent.eulerAngles + size;
		newComponent.entity = (Entity *)(newComponent.scale + size);

		memcpy(newComponent.position, transformComponent.position, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.eulerAngles, transformComponent.eulerAngles, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.scale, transformComponent.scale, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.entity, transformComponent.entity, transformComponent.instancesCount * sizeof(Entity));

		free(transformComponent.buffer);
		transformComponent = newComponent;
	}
}