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

	//uint32_t ResourceManagerSystem::CreateComponent(const Entity& e, ComponentType type, void* componentData)
	//{
	//	const auto& it = creationFunctions.find(type);

	//	if (it != creationFunctions.end())
	//	{
	//		return it->second(e, componentData);
	//	}

	//	return 0;
	//}

	//int32_t ResourceManagerSystem::FindComponentDataIndex(ComponentType type, int32_t componentsCount, const ComponentData* data)
	//{
	//	for (int32_t i = 0; i < componentsCount; ++i)
	//	{
	//		if (data[i].type == type)
	//		{
	//			return i;
	//		}
	//	}

	//	return -1;
	//}
}