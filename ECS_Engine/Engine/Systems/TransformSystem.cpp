#include "TransformSystem.h"
#include "WorkDispatcherSystem.h"

#include <assert.h>

TransformSystem::TransformSystem()
{
	AllocateMemory(DEFAULT_ALLOCATED_MEMORY);
}

TransformSystem::~TransformSystem()
{

}

void TransformSystem::Process(float deltaTime)
{
	std::vector<Task> tasks;

	for (uint32 i = 0; i < transformComponentData.instancesCount; i += MAX_THREAD_WORKLOAD)
	{
		uint32 endIndex = std::min(i + MAX_THREAD_WORKLOAD, (uint32)transformComponentData.instancesCount);

		tasks.emplace_back([this, deltaTime, i, endIndex]()
			{
				ProcessComponents(deltaTime, i, endIndex);
			});
	}

	WorkDispatcherSystem::Get().AddTasks(tasks);

	/*for (int i = 0; i < transformComponentData.instancesCount; ++i)
	{
		Move(i, Vector3(1.0f));
	}*/
}

void TransformSystem::ProcessComponents(float deltaTime, uint32 startIndex, uint32 endIndex)
{
	for (uint32 i = startIndex; i < endIndex; ++i)
	{
		Move(i, Vector3(1.0f));
	}
}

uint32 TransformSystem::CreateComponent(const Entity& e, void* componentData)
{
	const uint32 index = instances.size();

	if (transformComponentData.allocatedInstances <= index)
	{
		//Allocating twice the existing memory size
		AllocateMemory(index * 2);
	}

	instances.emplace(e.id, index);
	transformComponentData.instancesCount++;

	transformComponentData.position[index] = Vector3(0.0f);
	transformComponentData.eulerAngles[index] = Vector3(0.0f);
	transformComponentData.scale[index] = Vector3(1.0f);

	transformComponentData.entity[index] = e;

	return index;
}

bool TransformSystem::EntityHasComponent(const Entity& e) const
{
	return instances.find(e.id) != instances.end();
}

bool TransformSystem::GetComponent(const Entity& e, uint32& outComponent) const
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		outComponent = it->second;
		return true;
	}

	return false;
}

void TransformSystem::SetPosition(const Entity& e, const Vector3& position)
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		SetPosition(it->second, position);
	}
}

void TransformSystem::SetRotation(const Entity& e, const Vector3& eulerAngles)
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		SetRotation(it->second, eulerAngles);
	}
}

void TransformSystem::SetScale(const Entity& e, const Vector3& scale)
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		SetScale(it->second, scale);
	}
}

void TransformSystem::SetPosition(uint32 component, const Vector3& position)
{
	transformComponentData.position[component] = position;
}

void TransformSystem::SetRotation(uint32 component, const Vector3& eulerAngles)
{
	transformComponentData.eulerAngles[component] = eulerAngles;
}

void TransformSystem::SetScale(uint32 component, const Vector3& scale)
{
	transformComponentData.scale[component] = scale;
}

void TransformSystem::Move(const Entity& e, const Vector3& direction)
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		Move(it->second, direction);
	}
}

void TransformSystem::Rotate(const Entity& e, const Vector3& axis)
{
	const auto it = instances.find(e.id);

	if (it != instances.end())
	{
		Rotate(it->second, axis);
	}
}

void TransformSystem::Move(uint32 component, const Vector3& direction)
{
	transformComponentData.position[component] += direction;
}

void TransformSystem::Rotate(uint32 component, const Vector3& axis)
{
	transformComponentData.eulerAngles[component] += axis;
}

void TransformSystem::AllocateMemory(int32 size)
{
	assert(size > transformComponentData.allocatedInstances);

	TransformComponentData newData;

	const int32 sizeToAllocate = size * (sizeof(Vector3) * 3 + sizeof(Entity));

	newData.buffer = malloc(sizeToAllocate);

	newData.instancesCount = transformComponentData.instancesCount;
	newData.allocatedInstances = size;

	newData.position = (Vector3*)(newData.buffer);
	newData.eulerAngles = newData.position + size;
	newData.scale = newData.eulerAngles + size;
	newData.entity = (Entity*)(newData.scale + size);

	memcpy(newData.position, transformComponentData.position, transformComponentData.instancesCount * sizeof(Vector3));
	memcpy(newData.eulerAngles, transformComponentData.eulerAngles, transformComponentData.instancesCount * sizeof(Vector3));
	memcpy(newData.scale, transformComponentData.scale, transformComponentData.instancesCount * sizeof(Vector3));
	memcpy(newData.entity, transformComponentData.entity, transformComponentData.instancesCount * sizeof(Entity));

	free(transformComponentData.buffer);
	transformComponentData = newData;
}