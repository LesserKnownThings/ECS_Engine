#include "TilemapHelper.h"
#include "Components/GridComponent.h"

namespace LKT
{
    std::unordered_map<ECellType, TileTexture> TilemapHelper::tileTextures = {
        {ECellType::Grass, TileTexture{glm::vec2(0.f), glm::vec2(32.f)}},
        {ECellType::Dirt, TileTexture{glm::vec2(32.0f), glm::vec2(32.f)}}};

    bool TilemapHelper::GetTileTexture(ECellType type, TileTexture &outTileTexture)
    {
        const auto it = tileTextures.find(type);
        if (it != tileTextures.end())
        {
            outTileTexture = it->second;
            return true;
        }
        return false;
    }
}