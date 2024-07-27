#pragma once

#include "../Entity.h"
#include "../Math/Numerals.h"
#include "SystemManager.h"

#include <cstdlib>
#include <functional>
#include <unordered_map>

class ComponentSystemBase;

using CreationFunction = std::function<uint32(const Entity&, void*)>;

//16 components should be more than enough for now, if needed I can expand to 32, but I doubt I will do that in the near future

enum class ComponentType : uint16
{
	None = 0x0000,
	TransformComponent = 0x0001,
	RenderComponent	= 0x0002,
	PhysicsComponent = 0x004,
};

struct ComponentData
{
	~ComponentData()
	{
		if (componentData != nullptr)
			free(componentData);
	}

	Entity entity;
	ComponentType type;
	uint32 component;

	void* componentData = nullptr;
};

/// <summary>
/// entitiesCount -> how many entities to spawn
/// componentsToSpawn -> what components to spawn,
/// componentsCount -> how many components does the componentsToSpawn contain
/// entitiesIndex -> the index for each entity spawned for this resource
/// components -> the id of each component spawned for each entity in this resource
/// For example if you have 2 components type you can access them by doing components[index + componentOffset] the components are spawned in order so the component offset increases by one, so if you want to access the 2nd component data you will have to offset the data by the number of entities like this components[index + (2 * entitiesCount)]
/// </summary>
struct EntityResource
{
	~EntityResource()
	{
		delete[] entitiesIndex;
		delete[] components;
	}

	EntityResource(int32 count, int32 inComponentsCount, int32 inComponentsToSpawn, ComponentData* inComponentData)
		: entitiesCount(count), componentsCount(inComponentsCount), componentsToSpawn(inComponentsToSpawn), components(std::move(inComponentData))
	{
		entitiesIndex = new Entity[count];
	}

	int32 entitiesCount;
	int32 componentsCount;
	uint8 componentsToSpawn;

	Entity* entitiesIndex = nullptr;
	ComponentData* components = nullptr;
};

class ResourceManagerSystem
{
public:
	static ResourceManagerSystem& Get();

	void RegisterFunction(ComponentType type, CreationFunction func);

	//Spawns entities based on the EntityResource and stores the data in the same object
	void SpawnEntities(EntityResource& resource);

	uint32 CreateComponent(const Entity& e, ComponentType type, void* componentData);
private:
	//The function doesn't make sure the componentsCount is valid, so make sure to send the right amount
	int32 FindComponentDataIndex(ComponentType type, int32 componentsCount, const ComponentData* data);

	std::unordered_map<ComponentType, CreationFunction> creationFunctions;
};

#define REGISTER_CREATION_FUNCTION(system, componentType) \
	struct system##CompCreation \
	{ \
		system##CompCreation() \
		{ \
			if(system* s = SystemManager::Get().GetSystem<system>(typeid(system))) \
			{ \
				ResourceManagerSystem::Get().RegisterFunction(componentType, [s](const Entity& e, void* componentData) -> uint32 \
					{ \
						return s->CreateComponent(e, componentData); \
					}); \
			} \
		} \
	}; \
	static system##CompCreation system##CompCreation_;