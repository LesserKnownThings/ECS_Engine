#pragma once

#include "ComponentFactory.h"
#include "Entity.h"
#include "glm/glm.hpp"

#include <cstdint>
#include <string>

namespace LKT
{
    enum class ECellType : uint8_t
    {
        None = 0,
        Water,
        Grass,
        Dirt,
        Rock
    };

    struct GridComponent
    {
        int32_t instances;
        int32_t allocatedInstances;

        void *buffer = nullptr;

        Entity *entity = nullptr;

        glm::ivec2 *position = nullptr;
        ECellType *cellType = nullptr;
        bool *isTaken = nullptr;
    };

    struct GridComponentResource : public ComponentResource
    {
        GridComponentResource(int32_t instances,
                              glm::ivec2 *position = nullptr,
                              ECellType *cellType = nullptr,
                              bool *isTaken = nullptr);
    };
}