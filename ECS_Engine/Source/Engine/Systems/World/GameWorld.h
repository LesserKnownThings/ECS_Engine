#pragma once

#include "BuildMacros.h"
#include "Components/RenderComponent.h"
#include "glm/glm.hpp"
#include "Systems/AssetManager/LazyAssetPtr.h"

#if GAME_ONLY
#include "World.h"

namespace LKT
{
    class Asset;

    struct EntityResource;

    class GameWorld : public World
    {
        public:
        void Initialize() override;
        void Uninitialize() override;

        private:
        void Process(float deltaTime);
		void Render();

        void CreateBuffers();
		void CreateGrid();
		void CreateRenderData(const EntityResource &resource);

        glm::ivec2 CalculateVisibleGridBounds();

		std::vector<uint32_t> entities;
		
		Rect2D cameraRect;
        RenderComponentObject rco;
        LazyAssetPtr<Asset> textureAtlas;
    };
}
#endif