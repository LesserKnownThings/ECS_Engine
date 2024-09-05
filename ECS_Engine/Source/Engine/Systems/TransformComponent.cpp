#include "TransformComponent.h"

#include <cstring>

namespace LKT
{
	TransformComponentResource::TransformComponentResource(int32_t instances, glm::vec3* position, glm::vec3* eulerAngles, glm::vec3* scale)
	{
		auto func =
			[this, instances](const glm::vec3* position, const glm::vec3* eulerAngles, const glm::vec3* scale)
			{
				this->bufferSize = instances * sizeof(glm::vec3) * 3;

				this->buffer = malloc(this->bufferSize);

				glm::vec3* modBuffer = static_cast<glm::vec3*>(this->buffer);

				const uint32_t size = instances * sizeof(glm::vec3);

				if (position != nullptr)
				{
					memcpy((char*)modBuffer, position, size);
				}
				else
				{
					std::fill(modBuffer, modBuffer + instances, glm::vec3(0.0f));
				}

				if (eulerAngles != nullptr)
				{
					memcpy((char*)modBuffer + size, eulerAngles, size);
				}
				else
				{
					std::fill(modBuffer + instances, modBuffer + instances * 2, glm::vec3(0.0f));
				}

				if (scale != nullptr)
				{
					memcpy((char*)modBuffer + size * 2, scale, size);
				}
				else
				{
					std::fill(modBuffer + instances * 2, modBuffer + instances * 3, glm::vec3(1.0f));
				}
			};

		std::tuple<glm::vec3*, glm::vec3*, glm::vec3*> args(position, eulerAngles, scale);
		ComponentFactoryType<glm::vec3*, glm::vec3*, glm::vec3*> factory{ func, args };

		factory.CreateComponentData();
	}
}