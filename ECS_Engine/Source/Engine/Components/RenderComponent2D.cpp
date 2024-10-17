#include "RenderComponent2D.h"

#include <cstring>

namespace LKT
{
    SpriteComponentResource::SpriteComponentResource(int32_t instances,
                                                         const std::string &texturePath,
                                                         glm::vec2 *offset,
                                                         glm::vec2 *size)
    {
        auto func =
            [this, instances](glm::vec2 *offset, glm::vec2 *size)
        {
            this->bufferSize = instances * (sizeof(glm::vec2) * 2 + sizeof(uint32_t));
            this->buffer = malloc(this->bufferSize);

            glm::vec3 *modBuffer = static_cast<glm::vec3 *>(this->buffer);

            const uint32_t bufferSize = instances * sizeof(glm::vec3);

            if (offset != nullptr)
            {
                memcpy((char *)modBuffer, offset, bufferSize);
                delete[] offset;
            }
            else
            {
                std::fill(modBuffer, modBuffer + instances, glm::vec3(0.0f));
            }

            if (size != nullptr)
            {
                memcpy((char *)modBuffer + bufferSize, size, bufferSize);
                delete[] size;
            }
            else
            {
                std::fill(modBuffer + instances, modBuffer + instances * 2, glm::vec3(0.0f));
            }
        };

        std::tuple<glm::vec2 *, glm::vec2 *> args(offset, size);
        ComponentFactoryType<glm::vec2 *, glm::vec2 *> factory{func, args};

        const uint32_t texturePathSize = texturePath.size();
        commonData = malloc(texturePathSize + sizeof(uint32_t));
        char *commonModBuffer = reinterpret_cast<char *>(commonData);

        memcpy(commonModBuffer, &texturePath, sizeof(uint32_t));
        memcpy(commonModBuffer + sizeof(uint32_t), texturePath.data(), texturePathSize);

        factory.CreateComponentData();
    }
}