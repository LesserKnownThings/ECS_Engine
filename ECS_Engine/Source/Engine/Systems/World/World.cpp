#include "World.h"

namespace LKT
{
    void World::GiveAsset(LazyAssetPtr<Asset> &strongRefAsset)
    {
        assets.push_back(std::move(strongRefAsset));
    }

    World::World()
    {
    }

    void World::Render()
    {
    }

    void World::Process(float deltaTime)
    {
    }
}