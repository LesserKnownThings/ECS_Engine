#include "AssetManager.h"
#include "Asset.h"
#include "AssetFactory.h"
#include "FileHelper.h"
#include "GraphicHelpers.h"

#include <iostream>

namespace fs = std::filesystem;

namespace LKT
{
	AssetManager &AssetManager::Get()
	{
		static AssetManager instance;
		return instance;
	}

	void AssetManager::GetAsset(const AssetPath &inPath, AssetGetFunction func)
	{
		auto it = assets.find(inPath);

		if (it != assets.end())
		{
			func(it->second);
		}
	}

	uint32_t AssetManager::GetAssetTexture(const std::string &extension) const
	{
		const int32_t id = AssetFactory::GetAssetExtensionId(extension);
		if (id != 0)
		{
			const auto it = assetViewTextures.find(id);
			if (it != assetViewTextures.end())
			{
				return it->second;
			}
		}

		return 0;
	}

	uint32_t AssetManager::GetFolderTexture() const
	{
		const auto it = assetViewTextures.find(FOLDER);
		if (it != assetViewTextures.end())
		{
			return it->second;
		}

		return 0;
	}

	void AssetManager::BuildAssetRegistry()
	{
		std::vector<fs::path> assetPaths;
		FileHelper::GetFilesFromDirectory(contentFolder, assetPaths);

		for (const fs::path &assetPath : assetPaths)
		{
			AssetPath ap{assetPath};
			assets.emplace(ap, LazyAssetPtr<Asset>{ap});
		}

		LoadAssetViewTextures();
	}

	void AssetManager::LoadAssetViewTextures()
	{
		textureStorage =
			{
				// Attribute to https://www.flaticon.com/authors/freepik
				AssetTextureStorage{FOLDER, "Data/Textures/Assets/folder.png"},
				// Attribute to https://www.flaticon.com/authors/freepik
				AssetTextureStorage{TEXTURE, "Data/Textures/Assets/texture.png"},
				// Attribute to https://www.flaticon.com/authors/freepik
				AssetTextureStorage{MESH, "Data/Textures/Assets/mesh.png"},
			};

		for (const AssetTextureStorage &storage : textureStorage)
		{
			uint32_t textureID;
			int32_t width, height;

			GraphicHelpers::LoadTexture(storage.path, textureID, width, height);
			assetViewTextures[storage.id] = textureID;
		}
	}

	AssetPath::AssetPath(const std::filesystem::path &inFullPath)
	{
		fullPath = inFullPath.string();

		assetName = inFullPath.stem().string();
		extension = inFullPath.extension().string();

		for (const fs::path &path : inFullPath)
		{
			if (!path.has_filename())
			{
				subFolders.push_back(path.string());
			}
		}
	}
}