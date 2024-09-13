#include "ResourceManagerSystem.h"
#include "DebugSystem.h"

#include <iostream>

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
		factories[type] = [func](int32_t entityCount, Entity *entities, void *componentData)
		{
			func(entityCount, entities, componentData);
		};
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