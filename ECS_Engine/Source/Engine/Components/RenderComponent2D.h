#pragma once

#include "glm/glm.hpp"
#include "ComponentFactory.h"

#include <cstdint>
#include <string>

namespace LKT
{
    struct Entity;

    struct SpriteComponent
    {
        int32_t instances;
        int32_t allocatedInstances;

        void *buffer = nullptr;

        glm::vec2 *offset = nullptr;
        glm::vec2 *size = nullptr;

        Entity *entity = nullptr;

        uint32_t *vao = nullptr;
        uint32_t *vbo = nullptr;
        uint32_t *ebo = nullptr;
        uint32_t *elementCount = nullptr;
        uint32_t *textureId = nullptr;
    };

    struct SpriteComponentResource : public ComponentResource
    {
        // Allocate memory with [] instead of malloc because the destructor uses []
        SpriteComponentResource(int32_t instances,
                                  const std::string &texturePath,
                                  glm::vec2 *offset,
                                  glm::vec2 *size);
    };
}