#include "AssetFactory.h"
#include "Assets/Texture.h"

namespace LKT
{
	AssetFactory& AssetFactory::Get()
	{
		static AssetFactory instance;
		return instance;
	}

	//TODO refactor this. This is not exactly a good factory, I need to include all the assets that I want. I'm doing this for testing only
	AssetFactory::AssetFactory()
	{
		auto textureFactory = [](const std::string& path) -> Asset*
			{
				Asset* asset = new Texture();
				asset->LoadAsset(path);
				return asset;
			};

		factories =
		{
			{ typeid(Texture), textureFactory }
		};
	}
}
