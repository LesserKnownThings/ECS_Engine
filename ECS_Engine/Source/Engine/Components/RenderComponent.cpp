#include "RenderComponent.h"
#include "Systems/MeshLoadingSystem.h"

#include <cstring>

namespace LKT
{
	RenderComponentResource::RenderComponentResource(int32_t instances, uint32_t *textureID)
	{
		auto func =
			[this, instances](uint32_t *textures)
		{
			this->bufferSize = instances * sizeof(uint32_t) * 5;
			this->buffer = malloc(this->bufferSize);

			memset(buffer, 0, instances * sizeof(uint32_t) * 4);

			uint32_t *modBuffer = static_cast<uint32_t *>(this->buffer);
			if (textures == nullptr)
			{
				std::fill(modBuffer + instances * 4, modBuffer + instances * 5, 0);
			}
			else
			{
				memcpy((char *)modBuffer + 4 * instances * sizeof(uint32_t), textures, sizeof(uint32_t) * instances);
				delete[] textures;
			}
		};

		std::tuple<uint32_t *> args(textureID);

		ComponentFactoryType<uint32_t *> factory{func, args};
		factory.CreateComponentData();
	}
}