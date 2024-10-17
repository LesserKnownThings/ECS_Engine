#include "RenderSystem2D.h"
#include "AssetManager/Asset.h"
#include "AssetManager/AssetManager.h"
#include "Assets/Texture.h"
#include "EntityManager.h"
#include "HelperFunctions2D.h"
#include "glew/glew.h"

#include <cstring>

namespace LKT
{
    RenderSystem2D &RenderSystem2D::Get()
    {
        static RenderSystem2D instance;
        return instance;
    }

    RenderSystem2D::RenderSystem2D()
    {
        Allocate(DEFAULT_ALLOCATED_MEMORY);
    }

    void RenderSystem2D::HandleEntityRemoved(const Entity &e)
    {
        const auto c = spriteInstances.find(e);
        if (c != spriteInstances.end())
        {
            DestroyComponent(c->second);
        }
    }

    void RenderSystem2D::DestroyComponent(uint32_t c)
    {
        textureRefs.erase(renderComponent.textureId[c]);
        const uint32_t last = renderComponent.instances - 1;
        const Entity currentEntity = renderComponent.entity[c];
        const Entity lastEntity = renderComponent.entity[last];

        renderComponent.offset[c] = renderComponent.offset[last];
        renderComponent.size[c] = renderComponent.size[last];
        renderComponent.entity[c] = renderComponent.entity[last];
        renderComponent.vao[c] = renderComponent.vao[last];
        renderComponent.vbo[c] = renderComponent.vbo[last];
        renderComponent.ebo[c] = renderComponent.ebo[last];
        renderComponent.elementCount[c] = renderComponent.elementCount[last];
        renderComponent.textureId[c] = renderComponent.textureId[last];

        spriteInstances[lastEntity] = c;
        spriteInstances.erase(currentEntity);
        renderComponent.instances--;
    }

    void RenderSystem2D::CreateComponents(int32_t entityCount,
                                          Entity *entities,
                                          const std::type_index &type,
                                          void *commonData,
                                          void *componentData)
    {
        if (type == typeid(SpriteComponent))
        {
            CreateSpriteComponents(entityCount, entities, commonData, componentData);
        }
    }

    void RenderSystem2D::CreateSpriteComponents(int32_t entityCount,
                                                Entity *entities,
                                                void *commonData,
                                                void *componentData)
    {
        const uint32_t startingIndex = renderComponent.instances;

        if (renderComponent.allocatedInstances < entityCount)
        {
            Allocate(entityCount > startingIndex ? entityCount * 2 : startingIndex * 2);
        }

        renderComponent.instances += entityCount;

        for (int32_t i = 0; i < entityCount; ++i)
        {
            spriteInstances.emplace(entities[i].id, startingIndex + i);
        }

        char *modBuffer = static_cast<char *>(componentData);

        memcpy(&renderComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);

        uint32_t offset = 0;
        memcpy(&renderComponent.offset[startingIndex], modBuffer + offset, sizeof(glm::vec2) * entityCount);
        offset += sizeof(glm::vec2);
        memcpy(&renderComponent.size[startingIndex], modBuffer + offset, sizeof(glm::vec2) * entityCount);

        uint32_t *sizePtr = reinterpret_cast<uint32_t *>(commonData);
        uint32_t pathSize = *sizePtr;

        std::string texturePath(reinterpret_cast<char *>(sizePtr + 1));
        LazyAssetPtr<Asset> weakAsset;
        if (AssetManager::GetAsset(AssetPath{texturePath}, weakAsset))
        {
            for (int32_t i = 0; i < entityCount; ++i)
            {
                LazyAssetPtr<Asset> strongAsset = std::move(weakAsset.StrongRef());
                if (const Texture *texture = strongAsset.Get<Texture>())
                {
                    textureRefs.emplace(texture->GetTextureID(), std::move(weakAsset.StrongRef()));
                    CreateTextureData(startingIndex + i, texture);
                }
            }
        }
    }

    void RenderSystem2D::CreateTextureData(uint32_t c, const Texture *texture)
    {
        renderComponent.elementCount[c] = 6;
        renderComponent.textureId[c] = texture->GetTextureID();

        glCreateVertexArrays(1, &renderComponent.vao[c]);
        glCreateBuffers(1, &renderComponent.vbo[c]);
        glCreateBuffers(1, &renderComponent.ebo[c]);

        glBindVertexArray(renderComponent.vao[c]);
        glBindBuffer(GL_ARRAY_BUFFER, renderComponent.vbo[c]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderComponent.ebo[c]);

        UpdateTextureData(texture, renderComponent.offset[c], renderComponent.size[c]);
    }

    void RenderSystem2D::Allocate(int32_t size)
    {
        SpriteComponent copy;
        copy.instances = renderComponent.instances;
        copy.allocatedInstances = size;

        const uint32_t allocSize = size * (sizeof(glm::vec2) * 2 + sizeof(uint32_t) * 5 + sizeof(Entity));
        copy.buffer = malloc(allocSize);

        copy.offset = (glm::vec2 *)(copy.buffer);
        copy.size = copy.offset + size;
        copy.entity = (Entity *)(copy.size + size);
        copy.vao = (uint32_t *)(copy.entity + size);
        copy.vbo = copy.vao + size;
        copy.ebo = copy.vbo + size;
        copy.elementCount = copy.ebo + size;
        copy.textureId = copy.elementCount + size;

        memcpy(copy.offset, renderComponent.offset, copy.instances * sizeof(glm::vec2));
        memcpy(copy.size, renderComponent.size, copy.instances * sizeof(glm::vec2));
        memcpy(copy.entity, renderComponent.entity, copy.instances * sizeof(Entity));
        memcpy(copy.vao, renderComponent.vao, copy.instances * sizeof(uint32_t));
        memcpy(copy.vbo, renderComponent.vbo, copy.instances * sizeof(uint32_t));
        memcpy(copy.ebo, renderComponent.ebo, copy.instances * sizeof(uint32_t));
        memcpy(copy.elementCount, renderComponent.elementCount, copy.instances * sizeof(uint32_t));
        memcpy(copy.textureId, renderComponent.textureId, copy.instances * sizeof(uint32_t));

        free(renderComponent.buffer);
        renderComponent = copy;
    }
}
