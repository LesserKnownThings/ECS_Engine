#pragma once

#include <cstdint>
#include <string>

namespace LKT
{
    struct GraphicHelpers
    {
        static bool LoadTexture(const std::string &path, uint32_t &textureID, int32_t &width, int32_t &height);
    };
}