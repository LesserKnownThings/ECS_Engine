#pragma once

#include "glm/glm.hpp"
#include "Systems/AssetManager/Asset.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "Components/RenderComponent.h"

#include <cstdint>

namespace LKT
{
    class Texture;

    enum class ESpriteType : uint8_t
    {
        Single,
        Sliced
    };

    class SpriteAsset : public Asset
    {
    public:
        SpriteAsset();
        void UnloadAsset() override;

    private:
        void LoadAsset(void *buffer) override;
    };
}