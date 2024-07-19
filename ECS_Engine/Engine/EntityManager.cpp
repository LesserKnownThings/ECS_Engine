#include "EntityManager.h"

#include <assert.h>

EntityManager& EntityManager::Get()
{
    static EntityManager instance;

    return instance;
}

uint32 EntityManager::GetEntityIndex(const Entity& e) const
{
    return e.id & INDEX_MASK;
}

uint32 EntityManager::GetEntityGeneration(const Entity& e) const
{
    return (e.id >> INDEX_BITS) & GENERATION_MASK;
}

bool EntityManager::IsEntityAlive(const Entity& e) const
{
    return generation[GetEntityIndex(e)] == GetEntityGeneration(e);
}

Entity EntityManager::CreateEntity()
{
    uint32 index;

    if (free_indices.size() > MIN_FREE_INDICES)
    {
        index = free_indices.front();
        free_indices.pop_front();
    }
    else
    {
        generation.push_back(0);
        index = generation.size() - 1;

        assert(index < (static_cast<uint32>(1) << INDEX_BITS));
    }

    return Entity{ (static_cast<uint32>(generation[index]) << INDEX_BITS) | index };
}

void EntityManager::DestroyEntity(const Entity& e)
{
    const uint32 index = GetEntityIndex(e);
    generation[index]++;
    free_indices.push_back(index);
}