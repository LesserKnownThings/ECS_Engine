#include "Texture.h"
#include "GraphicHelpers.h"

#include <iostream>

namespace LKT
{
	void Texture::LoadAsset(void *buffer)
	{
		// GraphicHelpers::LoadTexture(path.fullPath, textureID, width, height);

		char *bufferMod = static_cast<char *>(buffer);

		memcpy(&width, bufferMod, sizeof(int32_t));
		memcpy(&height, bufferMod + sizeof(int32_t), sizeof(int32_t));
		memcpy(&components, bufferMod + sizeof(int32_t) * 2, sizeof(int32_t));

		void *pixels = static_cast<void *>(bufferMod + sizeof(int32_t) * 3);
		GraphicHelpers::LoadTexture(width, height, components, pixels, textureID);
	}

	void Texture::UnloadAsset()
	{
		GraphicHelpers::DeleteTexture(textureID);
	}

}