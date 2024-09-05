#include "ResourceManagerSystem.h"
#include "DebugSystem.h"

#include <iostream>

namespace LKT
{
	uint16_t ResourceManagerSystem::componentTypeIndex = 0;

	ResourceManagerSystem& ResourceManagerSystem::Get()
	{
		static ResourceManagerSystem instance;
		return instance;
	}

	void ResourceManagerSystem::RegisterFunction(std::type_index type, CreationFunction func)
	{
		factories[type] = [func](int32_t entityCount, Entity* entities, void* componentData)
			{
				func(entityCount, entities, componentData);
			};
	}
}