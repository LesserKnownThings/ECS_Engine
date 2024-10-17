#include "GridSystem.h"

#include <cstring>

namespace LKT
{
    GridSystem &GridSystem::Get()
    {
        static GridSystem instance;
        return instance;
    }

    GridSystem::GridSystem()
    {
        Allocate(DEFAULT_ALLOCATED_MEMORY);
    }

    void GridSystem::Allocate(int32_t size)
    {
        GridComponent copy;
        copy.instances = gridComponent.instances;
        copy.allocatedInstances = size;

        const uint32_t sizeToAllocate = size * (sizeof(Entity) + sizeof(glm::ivec2) + 2 * sizeof(uint8_t));
        copy.buffer = malloc(sizeToAllocate);

        copy.entity = (Entity *)copy.buffer;
        copy.position = (glm::ivec2 *)(copy.entity + size);
        copy.cellType = (ECellType *)(copy.position + size);
        copy.isTaken = (bool *)(copy.cellType + size);

        memcpy(copy.entity, gridComponent.entity, copy.instances * sizeof(Entity));
        memcpy(copy.position, gridComponent.position, copy.instances * sizeof(glm::ivec2));
        memcpy(copy.cellType, gridComponent.cellType, copy.instances * sizeof(ECellType));
        memcpy(copy.isTaken, gridComponent.isTaken, copy.instances * sizeof(bool));

        free(gridComponent.buffer);
        gridComponent = copy;
    }

    void GridSystem::CreateComponents(int32_t entityCount,
                                      Entity *entities,
                                      const std::type_index &type,
                                      void *commonData,
                                      void *componentData)
    {
        const uint32_t startingIndex = instances.size();

        if (gridComponent.allocatedInstances < entityCount)
        {
            Allocate(entityCount > startingIndex ? entityCount * 2 : startingIndex * 2);
        }

        gridComponent.instances += entityCount;

        for (int32_t i = 0; i < entityCount; ++i)
        {
            instances.emplace(entities[i], startingIndex + i);
        }

        memcpy(&gridComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);

        memcpy(&gridComponent.position[startingIndex], componentData, sizeof(glm::ivec2) * entityCount);

        memcpy(&gridComponent.cellType[startingIndex],
               ((char *)componentData + sizeof(glm::ivec2) * entityCount), sizeof(ECellType) * entityCount);

        memcpy(&gridComponent.isTaken[startingIndex],
               ((char *)componentData + (sizeof(glm::ivec2) + sizeof(ECellType)) * entityCount),
               sizeof(bool) * entityCount);
    }

    bool GridSystem::GetCellType(const Entity &e, ECellType &outCellType) const
    {
        const auto it = instances.find(e);
        if (it != instances.end())
        {
            outCellType = gridComponent.cellType[it->second];
            return true;
        }
        return false;
    }

    bool GridSystem::GetCellPosition(const Entity &e, glm::ivec2 &outCellPosition) const
    {
        const auto it = instances.find(e);
        if (it != instances.end())
        {
            outCellPosition = gridComponent.position[it->second];
            return true;
        }
        return false;
    }
}