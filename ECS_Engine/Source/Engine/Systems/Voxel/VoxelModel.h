#pragma once

#include "glm/glm.hpp"

#include <cstdint>
#include <string>

namespace LKT
{
    struct VoxelColor
    {
        uint8_t r, g, b, a;
    };

    struct VoxelData
    {
        int8_t x, y, z;
        int8_t colorIndex;
    };

    struct VoxelModel
    {
        uint32_t version = 0;
        glm::ivec3 size;
        uint32_t nVoxels = 0;
        VoxelData *voxels = nullptr;
        VoxelColor palette[1024];
    };
}