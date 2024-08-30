#include "TransformSystem.h"
#include "WorkDispatcherSystem.h"
#include "TaskManagerSystem.h"
#include "ShaderSystem/ShaderManager.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <assert.h>

#include <iostream>

namespace LKT
{
	TransformSystem::TransformSystem()
	{
		AllocateMemory(DEFAULT_ALLOCATED_MEMORY);

		TaskManagerSystem::Get().RegisterTask(this, &TransformSystem::Process);
	}

	TransformSystem::~TransformSystem()
	{

	}

	TransformSystem& TransformSystem::Get()
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
				{
					ProcessComponents(deltaTime, i, endIndex);
				});
		}

		WorkDispatcherSystem::Get().AddTasks(tasks);
	}

	void TransformSystem::ProcessComponents(float deltaTime, uint32_t startIndex, uint32_t endIndex)
	{
		
	}

	uint32_t TransformSystem::CreateComponent(const Entity& e, void* componentData)
	{
		const uint32_t index = instances.size();

		if (transformComponent.allocatedInstances <= index)
		{
			//Allocating twice the existing memory size
			AllocateMemory(index * 2);
		}

		instances.emplace(e.id, index);
		transformComponent.instancesCount++;

		transformComponent.position[index] = glm::vec3(0.0f);
		transformComponent.eulerAngles[index] = glm::vec3(0.0f);
		transformComponent.scale[index] = glm::vec3(1.0f);

		transformComponent.entity[index] = e;

		return index;
	}

	void TransformSystem::CreateComponents(int32_t entityCount, Entity* entities, void* componentData)
	{
		const uint32_t startingIndex = instances.size();

		if (transformComponent.allocatedInstances < entityCount)
		{
			AllocateMemory(entityCount > startingIndex ? entityCount * 2 : startingIndex * 2);
		}

		transformComponent.instancesCount += entityCount;

		for (int32_t i = 0; i < entityCount; ++i)
		{
			instances.emplace(entities[i].id, startingIndex + i);
		}

		memcpy(&transformComponent.position[startingIndex], componentData, sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.eulerAngles[startingIndex], ((char*)componentData + sizeof(glm::vec3) * entityCount), sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.scale[startingIndex], ((char*)componentData + sizeof(glm::vec3) * 2 * entityCount), sizeof(glm::vec3) * entityCount);
		memcpy(&transformComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);
	}

	bool TransformSystem::EntityHasComponent(const Entity& e) const
	{
		return instances.find(e.id) != instances.end();
	}

	bool TransformSystem::GetComponent(const Entity& e, uint32_t& outComponent) const
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			outComponent = it->second;
			return true;
		}

		return false;
	}

	bool TransformSystem::GetPosition(const Entity& e, glm::vec3& outPos) const
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			outPos = transformComponent.position[it->second];
			return true;
		}

		return false;
	}

	void TransformSystem::SetPosition(const Entity& e, const glm::vec3& position)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetPosition(it->second, position);
		}
	}

	void TransformSystem::SetRotation(const Entity& e, const glm::vec3& eulerAngles)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetRotation(it->second, eulerAngles);
		}
	}

	void TransformSystem::SetScale(const Entity& e, const glm::vec3& scale)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			SetScale(it->second, scale);
		}
	}

	void TransformSystem::SetPosition(uint32_t component, const glm::vec3& position)
	{
		transformComponent.position[component] = position;
	}

	void TransformSystem::SetRotation(uint32_t component, const glm::vec3& eulerAngles)
	{
		transformComponent.eulerAngles[component] = eulerAngles;
	}

	void TransformSystem::SetScale(uint32_t component, const glm::vec3& scale)
	{
		transformComponent.scale[component] = scale;
	}

	void TransformSystem::Move(const Entity& e, const glm::vec3& direction)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			Move(it->second, direction);
		}
	}

	void TransformSystem::Rotate(const Entity& e, const glm::vec3& axis)
	{
		const auto it = instances.find(e.id);

		if (it != instances.end())
		{
			Rotate(it->second, axis);
		}
	}

	void TransformSystem::Move(uint32_t component, const glm::vec3& direction)
	{
		transformComponent.position[component] += direction;
	}

	void TransformSystem::Rotate(uint32_t component, const glm::vec3& axis)
	{
		transformComponent.eulerAngles[component] += axis;
	}

	//TODO move the model to the component data, it will be better to set the model when the pos/rot/scale change instead of calculating a model every frame
	bool TransformSystem::GetModel(const Entity& e, glm::mat4& outModel) const
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
		assert(size > transformComponent.allocatedInstances);

		TransformComponent newComponent;

		const int32_t sizeToAllocate = size * (sizeof(glm::vec3) * 3 + sizeof(Entity));

		newComponent.buffer = malloc(sizeToAllocate);

		newComponent.instancesCount = transformComponent.instancesCount;
		newComponent.allocatedInstances = size;

		newComponent.position = (glm::vec3*)(newComponent.buffer);
		newComponent.eulerAngles = newComponent.position + size;
		newComponent.scale = newComponent.eulerAngles + size;
		newComponent.entity = (Entity*)(newComponent.scale + size);

		memcpy(newComponent.position, transformComponent.position, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.eulerAngles, transformComponent.eulerAngles, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.scale, transformComponent.scale, transformComponent.instancesCount * sizeof(glm::vec3));
		memcpy(newComponent.entity, transformComponent.entity, transformComponent.instancesCount * sizeof(Entity));

		free(transformComponent.buffer);
		transformComponent = newComponent;
	}
}