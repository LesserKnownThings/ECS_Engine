#include "AssetManager.h"
#include "Asset.h"
#include "AssetData.h"
#include "AssetFactory/AssetFactory.h"
#include "AssetFactory/AssetFactoryManager.h"
#include "FileHelper.h"
#include "GraphicHelpers.h"
#include "LazyAssetPtr.h"

#if EDITOR_ONLY
#include "EditorUI/AssetViewerWindow.h"
#endif

#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace LKT
{
	AssetManager &AssetManager::Get()
	{
		static AssetManager instance;
		return instance;
	}

	bool AssetManager::GetAsset(const AssetPath &path, std::function<void(const LazyAssetPtr<Asset> &asset)> assetProcessFunc)
	{
		const AssetManager &instance = AssetManager::Get();
		const auto it = instance.assets.find(path);
		if (it != instance.assets.end())
		{
			assetProcessFunc(it->second.asset);
			return true;
		}

		return false;
	}

	Asset *AssetManager::LoadAsset(const AssetPath &path)
	{
		const AssetManager &instance = AssetManager::Get();
		const auto it = instance.assets.find(path);
		if (it != instance.assets.end())
		{
			return AssetFactoryManager::LoadAssetFactory(it->second.metadata);
		}
		return nullptr;
	}

	void AssetManager::BuildAssetRegistry()
	{
		AssetFactoryManager::LoadSupportedExtensions();

		std::vector<fs::path> assetPaths;
		FileHelper::GetFilesFromDirectory(contentFolder, assetPaths, ".asset");

		for (const fs::path &assetPath : assetPaths)
		{
			AssetPath ap{assetPath};
			AssetMetadata metadata;
			AssetFactoryManager::LoadMetadataHeader(ap, metadata);
			assets.emplace(ap, AssetData{ap, metadata});
		}

		LoadAssetViewTextures();
	}

#if EDITOR_ONLY
	void AssetManager::CreateAsset(const std::string &currentFolderPath, uint32_t type)
	{
		// uint32_t index = 1;
		// std::ostringstream oss;
		// oss << currentFolderPath << "/NewAsset_" << index << ".asset";
		// std::string filePath = oss.str();
		// oss.str("");

		// AssetPath path{filePath};
		// while (assets.contains(path))
		// {
		// 	++index;
		// 	oss << currentFolderPath << "/NewAsset_" << index << ".asset";
		// 	filePath = oss.str();
		// 	path = AssetPath{filePath};
		// 	oss.str("");
		// }

		// std::ofstream stream(filePath, std::ios::binary);
		// AssetMetadata metadata{type, static_cast<int32_t>(filePath.size()), filePath};
		// // WriteAssetMetadataStream(metadata, stream);
		// stream.close();

		// assets.emplace(path, LazyAssetPtr<Asset>(path));
	}

	void AssetManager::ImportAsset(const std::string &path, const std::string &currentFolder)
	{
		AssetManager &instance = AssetManager::Get();

		std::vector<AssetData> foundData;
		if (AssetFactoryManager::ImportAssetFactory(path, currentFolder, foundData))
		{
			for (const AssetData &data : foundData)
			{
				instance.assets.emplace(data.asset.GetPath(), data);
			}
		}
	}

	AssetViewerWindow *AssetManager::LoadAssetViewer(const AssetPath &path)
	{
		const AssetManager &instance = AssetManager::Get();
		const auto it = instance.assets.find(path);
		if (it != instance.assets.end())
		{
			return AssetFactoryManager::LoadAssetViewerFactory(it->second.metadata, path);
		}
		return nullptr;
	}

	uint32_t AssetManager::GetAssetTexture(const AssetPath &path)
	{
		const AssetManager &instance = AssetManager::Get();
		const auto it = instance.assets.find(path);

		if (it != instance.assets.end())
		{
			const auto textureIt = instance.assetViewTextures.find(it->second.metadata.type);
			if (textureIt != instance.assetViewTextures.end())
			{
				return textureIt->second;
			}
		}
		return 0;
	}

	uint32_t AssetManager::GetFolderTexture()
	{
		const AssetManager &instance = AssetManager::Get();

		const auto it = instance.assetViewTextures.find(FOLDER);
		if (it != instance.assetViewTextures.end())
		{
			return it->second;
		}

		return 0;
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
				// Attribute to https://www.flaticon.com/authors/freepik
				AssetTextureStorage{PARTICLESYSTEM, "Data/Textures/Assets/particle_system.png"},
			};

		for (const AssetTextureStorage &storage : textureStorage)
		{
			uint32_t textureID;
			int32_t width, height;

			GraphicHelpers::LoadTexture(storage.path, textureID, width, height);
			assetViewTextures[storage.id] = textureID;
		}
	}
#endif
}