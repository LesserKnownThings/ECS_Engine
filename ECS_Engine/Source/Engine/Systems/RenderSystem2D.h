#pragma once

#include "AssetManager/LazyAssetPtr.h"
#include "Components/RenderComponent2D.h"
#include "ResourceManagerSystem.h"
#include "SystemBase.h"

#include <unordered_map>

namespace LKT
{
    class Asset;
    class Texture;

    struct Entity;

    class RenderSystem2D : public SystemBase
    {
    public:
        static RenderSystem2D &Get();

        void CreateComponents(int32_t entityCount,
                              Entity *entities,
                              const std::type_index &type,
                              void *commonData,
                              void *componentData) override;

    private:
        RenderSystem2D();

        void CreateSpriteComponents(int32_t entityCount, Entity *entities, void *commonData, void *componentData);

        void HandleEntityRemoved(const Entity &e);
        void DestroyComponent(uint32_t c);

        void Allocate(int32_t size);
        SpriteComponent renderComponent;

        void CreateTextureData(uint32_t c, const Texture *texture);

        std::unordered_map<Entity, uint32_t> spriteInstances;

        /**
         * Holds strong refs for texture assets so it can load/unload textures
         * The key is the texture id not the component id
         */
        std::unordered_map<uint32_t, LazyAssetPtr<Asset>> textureRefs;
    };

    REGISTER_COMPONENT(RenderSystem2D, SpriteComponentResource)
}