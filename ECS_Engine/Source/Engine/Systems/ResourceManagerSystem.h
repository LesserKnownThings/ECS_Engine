#pragma once

#include "Components/ComponentFactory.h"
#include "Entity.h"
#include "EntityManager.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>

class ComponentSystemBase;

namespace LKT
{
	constexpr uint16_t INVALID_COMPONENT = 0;

	using CreationFunction = std::function<void(int32_t, Entity *, const std::type_index &, void *, void *)>;

	struct ComponentResourceData
	{
		ComponentResourceData() = default;
		ComponentResourceData(ComponentResourceData &&other);
		void *data = nullptr;
		void *commonData = nullptr;
	};

	struct EntityResource
	{
		EntityResource(int32_t inEntitiesCount)
			: entitiesCount(inEntitiesCount)
		{
			entities = (Entity *)malloc(entitiesCount * sizeof(Entity));
		}

		~EntityResource();

		EntityResource(EntityResource &&other);
		EntityResource &operator=(EntityResource &&other);

		EntityResource(const EntityResource &other) = delete;

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
		void SpawnEntities(EntityResource &resource);

	private:
		void CreateComponents(EntityResource &resource);

		std::unordered_map<std::type_index, CreationFunction> factories;

		// Used to index all components in the project
		static uint16_t componentTypeIndex;
	};

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
			ComponentResourceData comp;
			comp.data = compRes->buffer;
			comp.commonData = compRes->commonData;
			compRes->commonData = nullptr;
			compRes->buffer = nullptr;
			components.emplace(typeid(T), std::move(comp));
		}
	}

	// Bootleg reflection system
#define REGISTER_COMPONENT(type, compRes)                                                                                          \
	struct type##ComponentRegistrar                                                                                                \
	{                                                                                                                              \
		type##ComponentRegistrar()                                                                                                 \
		{                                                                                                                          \
			type &instance = type::Get();                                                                                          \
			ResourceManagerSystem::Get().RegisterFunction(typeid(compRes), [&instance](int32_t entityCount,                        \
																					   Entity *e,                                  \
																					   const std::type_index &type,                \
																					   void *commonData,                           \
																					   void *data)                                 \
														  { instance.CreateComponents(entityCount, e, type, commonData, data); }); \
		}                                                                                                                          \
	};                                                                                                                             \
	static type##ComponentRegistrar type##RegistrarInstance;
}