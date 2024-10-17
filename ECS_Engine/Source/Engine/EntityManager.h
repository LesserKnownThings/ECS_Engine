#pragma once

#include "Delegate.h"
#include "Entity.h"

#include <deque>
#include <vector>

namespace LKT
{
	constexpr uint32_t INDEX_BITS = 24;
	constexpr uint32_t INDEX_MASK = (static_cast<uint32_t>(1) << INDEX_BITS) - static_cast<uint32_t>(1);

	constexpr uint32_t GENERATION_BITS = 8;
	constexpr uint32_t GENERATION_MASK = (static_cast<uint32_t>(1) << GENERATION_BITS) - static_cast<uint32_t>(1);

	constexpr uint16_t MIN_FREE_INDICES = 1024;

	DECLARE_DELEGATE_OneParam(OnEntityRemoved, const Entity &);

	class EntityManager
	{
	public:
		static EntityManager &Get();

		uint32_t GetEntityIndex(const Entity &e) const;
		uint32_t GetEntityGeneration(const Entity &e) const;

		bool IsEntityAlive(const Entity &e) const;

		Entity CreateEntity();
		void DestroyEntity(const Entity &e);

		/**
		 * Should only be used on components that hold resources
		 * If component has no resources leave it to the gc
		 */
		OnEntityRemoved onEntityRemoved;

	private:
		std::vector<uint16_t> generation;
		std::deque<uint32_t> free_indices;
	};
}