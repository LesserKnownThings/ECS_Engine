#include "GridComponent.h"

#include <cstring>

namespace LKT
{
    GridComponentResource::GridComponentResource(int32_t instances,
                                                 glm::ivec2 *position,
                                                 ECellType *cellType,
                                                 bool *isTaken)
    {
        auto func =
            [this, instances](glm::ivec2 *position, ECellType *cellType, bool *isTaken)
        {
            this->bufferSize = instances * (sizeof(glm::ivec2) + sizeof(uint8_t) + sizeof(bool));

            this->buffer = malloc(this->bufferSize);

            if (position != nullptr)
            {
                memcpy((char *)this->buffer, position, sizeof(glm::ivec2) * instances);
                delete[] position;
            }
            else
            {
                glm::ivec2 *modBuffer = (glm::ivec2 *)this->buffer;
                std::fill(modBuffer, modBuffer + instances, glm::ivec2(0.0f));
            }

            uint32_t offset = sizeof(glm::ivec2);

            if (cellType != nullptr)
            {
                memcpy((char *)this->buffer + offset * instances, cellType, sizeof(ECellType) * instances);
                delete[] cellType;
            }
            else
            {
                uint8_t *modBuffer = (uint8_t *)this->buffer;
                std::fill(modBuffer + offset * instances, modBuffer + instances * (offset + sizeof(uint8_t)), 0);
            }

            offset += sizeof(uint8_t);

            if (isTaken != nullptr)
            {
                memcpy((char *)this->buffer + offset * instances, isTaken, sizeof(uint8_t) * instances);
                delete[] isTaken;
            }
            else
            {
                bool *modBuffer = (bool *)this->buffer;
                std::fill(modBuffer + instances * offset, modBuffer + instances * (offset + sizeof(bool)), false);
            }
        };

        std::tuple<glm::ivec2 *, ECellType *, bool *> args(position, cellType, isTaken);
        ComponentFactoryType<glm::ivec2 *, ECellType *, bool *> factory{func, args};

        factory.CreateComponentData();
    }
}