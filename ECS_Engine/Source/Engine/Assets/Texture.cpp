#include "Texture.h"
#include "GraphicHelpers.h"

namespace LKT
{
	void Texture::LoadAsset(const AssetPath &inPath)
	{
		Asset::LoadAsset(inPath);
		GraphicHelpers::LoadTexture(path.fullPath, textureID, width, height);
	}
}