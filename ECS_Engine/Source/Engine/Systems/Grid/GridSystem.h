#pragma once

#include "Entity.h"
#include "glm/glm.hpp"
#include "Systems/SystemBase.h"
#include "Components/GridComponent.h"
#include "Systems/ResourceManagerSystem.h"

#include <unordered_map>

namespace LKT
{
    class GridSystem : public SystemBase
    {
    public:
        static GridSystem &Get();

        void CreateComponents(int32_t entityCount,
                              Entity *entities,
                              const std::type_index &type,
                              void *commonData,
                              void *componentData) override;

        bool GetCellType(const Entity &e, ECellType &outCellType) const;
        bool GetCellPosition(const Entity &e, glm::ivec2 &outCellPosition) const;

    private:
        GridSystem();

        void Allocate(int32_t size);

        GridComponent gridComponent;
        std::unordered_map<Entity, uint32_t> instances;
    };

    REGISTER_COMPONENT(GridSystem, GridComponentResource)
}