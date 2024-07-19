#pragma once

#include "Entity.h"

#include <vector>
#include <deque>

constexpr uint32 INDEX_BITS = 24;
constexpr uint32 INDEX_MASK = (static_cast<uint32>(1) << INDEX_BITS) - static_cast<uint32>(1);

constexpr uint32 GENERATION_BITS = 8;
constexpr uint32 GENERATION_MASK = (static_cast<uint32>(1) << GENERATION_BITS) - static_cast<uint32>(1);

constexpr uint16 MIN_FREE_INDICES = 1024;

class EntityManager
{
public:
	static EntityManager& Get();

	uint32 GetEntityIndex(const Entity& e) const;
	uint32 GetEntityGeneration(const Entity& e) const;

	bool IsEntityAlive(const Entity& e) const;

	Entity CreateEntity();
	void DestroyEntity(const Entity& e);

private:
	std::vector<uint16> generation;
	std::deque<uint32> free_indices;
};