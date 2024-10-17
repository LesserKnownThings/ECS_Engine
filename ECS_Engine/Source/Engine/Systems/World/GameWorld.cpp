#include "GameWorld.h"
#if GAME_ONLY
#include "Assets/Texture.h"
#include "Camera.h"
#include "Components/GridComponent.h"
#include "glew/glew.h"
#include "Random.h"
#include "Systems/Grid/GridSystem.h"
#include "Systems/OpenGLSystem.h"
#include "Systems/ResourceManagerSystem.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/TaskManagerSystem.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/Grid/TilemapHelper.h"

#include <iostream>
#include <thread>

namespace LKT
{
    void RandomCellTypeThread(uint32_t startingIndex, uint32_t size, ECellType *&types)
    {
        Random::Seed();
        for (uint32_t i = startingIndex; i < size + startingIndex; ++i)
        {
            types[i] = (ECellType)Random::RandomRange(1, 4);
        }
    }

    void GameWorld::Initialize()
    {
        World::Initialize();

        CreateBuffers();

        TaskManagerSystem::Get().RegisterTask(this, &GameWorld::Render, 0, RENDER_HANDLE);

        /*250 terrain size*/
        constexpr int32_t entityCount = 62500;
        constexpr int32_t cellSize = 5;

        EntityResource res(entityCount);

        ECellType *types = new ECellType[entityCount];

        const uint32_t maxCores = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(maxCores);

        uint32_t workPerThread = entityCount / maxCores;
        const uint32_t remainder = entityCount - (workPerThread * maxCores);

        glm::ivec2 *positions = new glm::ivec2[entityCount];

        for (int32_t x = 0; x < 250; ++x)
        {
            for (int32_t y = 0; y < 250; ++y)
            {
                const glm::ivec2 pos(x * cellSize, y * cellSize);
                const int32_t index = y * 250 + x;
                positions[index] = pos;
            }
        }

        for (uint32_t i = 0; i < maxCores; ++i)
        {
            const uint32_t startingIndex = i * workPerThread;
            if (i == maxCores - 1)
            {
                workPerThread += remainder;
            }

            threads[i] = std::thread(RandomCellTypeThread, startingIndex, workPerThread, std::ref(types));
        }

        for (uint32_t i = 0; i < maxCores; ++i)
        {
            threads[i].join();
        }

        res.AddComponentResources<GridComponentResource>(
            GridComponentResource{entityCount, positions, types});

        ResourceManagerSystem::Get().SpawnEntities(res);

        CreateRenderData(res);
    }

    void GameWorld::Uninitialize()
    {
        glDeleteVertexArrays(1, &rco.vao);
        glDeleteBuffers(1, &rco.vbo);
        glDeleteBuffers(1, &rco.ebo);

        World::Uninitialize();
    }

    void GameWorld::Process(float deltaTime)
    {
    }

    void GameWorld::Render()
    {
        ShaderManager::Get().ActivateShader("Grid_Lit");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rco.textureID);
        ShaderManager::Get().SetInt("textureAtlas", rco.textureID);

        glBindVertexArray(rco.vao);
        glDrawElements(GL_TRIANGLES, rco.elementCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void GameWorld::CreateBuffers()
    {
        auto func = [this](const LazyAssetPtr<Asset> &asset)
        {
            textureAtlas = std::move(asset.StrongRef());
        };

        if (AssetManager::GetAsset(AssetPath{"Content/texture_atlas.asset"}, func))
        {
            if (const Texture *texture = textureAtlas.Get<Texture>())
            {
                rco.textureID = texture->GetTextureID();
            }
        }

        glCreateVertexArrays(1, &rco.vao);
        glCreateBuffers(1, &rco.vbo);
        glCreateBuffers(1, &rco.ebo);
    }

    void GameWorld::CreateGrid()
    {
        constexpr int32_t gridSize = 32;
    }

    void GameWorld::CreateRenderData(const EntityResource &resource)
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec2 texCoords;
        };

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (int32_t i = 0; i < resource.entitiesCount; ++i)
        {
            glm::ivec2 pos;
            GridSystem::Get().GetCellPosition(resource.entities[i], pos);

            const float cellSize = 5.0f;
            const float startX = pos.x;
            const float startY = pos.y;

            ECellType type;
            GridSystem::Get().GetCellType(resource.entities[i], type);

            TileTexture tileTexture;
            TilemapHelper::GetTileTexture(type, tileTexture);
            const glm::vec2 textureOffset = tileTexture.position;
            const glm::vec2 textureSize = tileTexture.size;

            vertices.push_back({glm::vec3(startX, startY, 0), glm::vec2(textureOffset.x, textureOffset.y)});
            vertices.push_back({glm::vec3(startX + cellSize, startY, 0), glm::vec2(textureOffset.x + textureSize.x, textureOffset.y)});
            vertices.push_back({glm::vec3(startX + cellSize, startY + cellSize, 0), glm::vec2(textureOffset.x + textureSize.x, textureOffset.y + textureSize.y)});
            vertices.push_back({glm::vec3(startX, startY + cellSize, 0), glm::vec2(textureOffset.x, textureOffset.y + textureSize.y)});

            // Calculate the indices for the two triangles that form the quad
            uint32_t topLeft = ((pos.y / 5) * 250 + (pos.x / 5)) * 4;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomRight = topLeft + 2;
            uint32_t bottomLeft = topLeft + 3;

            // First triangle (top-left, top-right, bottom-right)
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);

            // Second triangle (top-left, bottom-right, bottom-left)
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }

        glBindVertexArray(rco.vao);
        glBindBuffer(GL_ARRAY_BUFFER, rco.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rco.ebo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

        glBindVertexArray(0);

        rco.elementCount = indices.size();
    }

    glm::ivec2 GameWorld::CalculateVisibleGridBounds()
    {
        return glm::ivec2();
    }
}
#endif