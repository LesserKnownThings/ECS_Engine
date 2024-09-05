#pragma once

#include "ComponentFactory.h"
#include "Entity.h"
#include "EntityManager.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class ComponentSystemBase;

namespace LKT
{
	constexpr uint16_t INVALID_COMPONENT = 0;

	using CreationFunction = std::function<void(int32_t, Entity *, void *)>;

	struct ComponentResourceData
	{
		ComponentResourceData() = default;

		uint32_t component = 0;
		void *data = nullptr;
	};

	struct EntityResource
	{
		EntityResource(int32_t inEntitiesCount)
			: entitiesCount(inEntitiesCount)
		{
			entities = (Entity *)malloc(entitiesCount * sizeof(Entity));
		}

		template <typename... Args>
		void AddComponentResources(Args &&...args);

		int32_t entitiesCount = 0;

		Entity *entities = nullptr;

		std::unordered_map<std::type_index, ComponentResourceData> components;

	private:
		template <typename T>
		void AddComponentResource(T res);
	};

	class ResourceManagerSystem
	{
	public:
		static ResourceManagerSystem &Get();

		void RegisterFunction(std::type_index type, CreationFunction func);

		// Spawns entities based on the EntityResource and stores the data in the same object
		template <typename... Args>
		void SpawnEntities(EntityResource &resource);

	private:
		template <typename T>
		void CreateComponents(EntityResource &resource);

		std::unordered_map<std::type_index, CreationFunction> factories;

		// Used to index all components in the project
		static uint16_t componentTypeIndex;
	};

	template <typename T>
	void ResourceManagerSystem::CreateComponents(EntityResource &resource)
	{
		auto it = factories.find(typeid(T));

		if (it != factories.end())
		{
			it->second(resource.entitiesCount, resource.entities, resource.components[typeid(T)].data);
		}
	}

	template <typename... Args>
	void ResourceManagerSystem::SpawnEntities(EntityResource &resource)
	{
		for (int32_t i = 0; i < resource.entitiesCount; ++i)
		{
			resource.entities[i] = EntityManager::Get().CreateEntity();
		}

		(CreateComponents<Args>(resource), ...);
	}

	template <typename... Args>
	void EntityResource::AddComponentResources(Args &&...args)
	{
		(AddComponentResource(std::forward<Args>(args)), ...);
	}

	template <typename T>
	void EntityResource::AddComponentResource(T res)
	{
		if (ComponentResource *compRes = reinterpret_cast<ComponentResource *>(&res))
		{
			ComponentResourceData compData;
			compData.data = std::move(compRes->buffer);
			components.emplace(typeid(T), compData);
		}
	}

	// Bootleg reflection system
#define REGISTER_COMPONENT(type, compRes)                                                                                          \
	struct type##ComponentRegistrar                                                                                                \
	{                                                                                                                              \
		type##ComponentRegistrar()                                                                                                 \
		{                                                                                                                          \
			type &instance = type::Get();                                                                                          \
			ResourceManagerSystem::Get().RegisterFunction(typeid(compRes), [&instance](int32_t entityCount, Entity *e, void *data) \
														  { instance.CreateComponents(entityCount, e, data); });                   \
		}                                                                                                                          \
	};                                                                                                                             \
	static type##ComponentRegistrar type##RegistrarInstance;
}