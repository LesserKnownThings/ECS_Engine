#pragma once

#include "glm/glm.hpp"

#include <unordered_map>

namespace LKT
{
    enum class ECellType : uint8_t;

    struct TileTexture
    {
        glm::vec2 position;
        glm::vec2 size;
    };

    class TilemapHelper
    {
    public:
        static bool GetTileTexture(ECellType type, TileTexture &outTileTexture);

    private:
        static std::unordered_map<ECellType, TileTexture> tileTextures;
    };
}