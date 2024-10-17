#include "ResourceManagerSystem.h"
#include "DebugSystem.h"

#include <iostream>
#include <thread>

namespace LKT
{
	uint16_t ResourceManagerSystem::componentTypeIndex = 0;

	ResourceManagerSystem &ResourceManagerSystem::Get()
	{
		static ResourceManagerSystem instance;
		return instance;
	}

	void ResourceManagerSystem::RegisterFunction(std::type_index type, CreationFunction func)
	{
		factories[type] = [func](int32_t entityCount,
								 Entity *entities,
								 const std::type_index &type,
								 void *commonData,
								 void *componentData)
		{
			func(entityCount, entities, type, commonData, componentData);
		};
	}

	void ResourceManagerSystem::SpawnEntities(EntityResource &resource)
	{
		for (int32_t i = 0; i < resource.entitiesCount; ++i)
		{
			resource.entities[i] = EntityManager::Get().CreateEntity();
		}

		CreateComponents(resource);
	}

	void ResourceManagerSystem::CreateComponents(EntityResource &resource)
	{
		for (const auto &comp : resource.components)
		{
			auto it = factories.find(comp.first);

			if (it != factories.end())
			{
				it->second(resource.entitiesCount,
						   resource.entities,
						   comp.first,
						   resource.components[comp.first].commonData,
						   resource.components[comp.first].data);
			}
		}
	}

	EntityResource::~EntityResource()
	{
		free(entities);

		for (auto &it : components)
		{
			delete it.second.data;
			it.second.data = nullptr;
		}
	}

	EntityResource::EntityResource(EntityResource &&other)
		: entitiesCount(other.entitiesCount), entities(other.entities), components(std::move(other.components))
	{
		other.entities = nullptr;
	}

	EntityResource &EntityResource::operator=(EntityResource &&other)
	{
		if (this != &other)
		{
			entitiesCount = other.entitiesCount;
			entities = other.entities;
			other.entities = nullptr;
			components = std::move(other.components);
		}
	}

	ComponentResourceData::ComponentResourceData(ComponentResourceData &&other)
		: data(other.data)
	{
		other.data = nullptr;
	}
}