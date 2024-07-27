#include "ResourceManagerSystem.h"
#include "../EntityManager.h"
#include "DebugSystem.h"

#include <iostream>

ResourceManagerSystem& ResourceManagerSystem::Get()
{
	static ResourceManagerSystem instance;
	return instance;
}

void ResourceManagerSystem::RegisterFunction(ComponentType type, CreationFunction func)
{
	creationFunctions[type] = [func](const Entity& e, void* componentData) -> uint32
		{
			return func(e, componentData);
		};
}

void ResourceManagerSystem::SpawnEntities(EntityResource& resource)
{
	for (int32 i = 0; i < resource.entitiesCount; ++i)
	{
		resource.entitiesIndex[i] = EntityManager::Get().CreateEntity();
	}

	int32 compOffset = 0;

	for (uint16 c = 0; c < 16; ++c)
	{
		uint16 mask = 1 << c;

		if (resource.componentsToSpawn & mask)
		{
			for (int32 i = 0; i < resource.entitiesCount; ++i)
			{
				ComponentType type = static_cast<ComponentType>(mask);

				int32 compIndex = FindComponentDataIndex(type, resource.componentsCount, resource.components);

				if (compIndex != -1)
				{
					uint32 comp = CreateComponent(resource.entitiesIndex[i], type, resource.components[compIndex].componentData);

					resource.components[compIndex] = { resource.entitiesIndex[i], type, comp };
				}
				else
				{
					//TODO add debug with args
					//DebugSystem::Log(DebugLevel::Error, "Could not find component of type: ")
					std::cerr << "Error: Could not find component of type: " << mask << std::endl;
				}
			}

			compOffset++;
		}
	}
}

uint32 ResourceManagerSystem::CreateComponent(const Entity& e, ComponentType type, void* componentData)
{
	const auto& it = creationFunctions.find(type);

	if (it != creationFunctions.end())
	{
		return it->second(e, componentData);
	}

	return 0;
}

int32 ResourceManagerSystem::FindComponentDataIndex(ComponentType type, int32 componentsCount, const ComponentData* data)
{
	for (int32 i = 0; i < componentsCount; ++i)
	{
		if (data[i].type == type)
		{
			return i;
		}
	}

	return -1;
}