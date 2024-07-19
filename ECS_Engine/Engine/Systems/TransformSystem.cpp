#include "TransformSystem.h"

#include <assert.h>

TransformSystem::TransformSystem()
{
	AllocateMemory(DEFAULT_ALLOCATED_MEMORY);
}

TransformSystem::~TransformSystem()
{

}

uint32 TransformSystem::AddComponent(const Entity& e)
{
	const uint32 index = instances.size();

	if (transformComponentData.allocatedInstances <= index)
	{
		//Allocating twice the existing memory size
		AllocateMemory(index * 2);
	}

	instances.emplace(e.id, index);

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

void TransformSystem::AllocateMemory(int32 size)
{
	assert(size > transformComponentData.allocatedInstances);

	TransformComponentData newData;

	const int32 sizeToAllocate = size * (sizeof(Vector3) * 3) + sizeof(Entity);
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