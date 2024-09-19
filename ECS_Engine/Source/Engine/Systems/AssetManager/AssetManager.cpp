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

	void AssetManager::Initialize()
	{
		AssetFactoryManager::LoadSupportedExtensions();
#if EDITOR_ONLY
		ImportEngineAssets();
#endif
		BuildRegistries();
	}

	void AssetManager::BuildRegistries()
	{
		BuildEngineAssetRegistry();
		BuildAssetRegistry();
	}

	void AssetManager::BuildEngineAssetRegistry()
	{
		BuildRegistry(engineFolder);
	}

	void AssetManager::BuildAssetRegistry()
	{
		BuildRegistry(contentFolder);
	}

	void AssetManager::BuildRegistry(const std::string &path)
	{
		std::vector<fs::path> assetPaths;
		FileHelper::GetFilesFromDirectory(path, assetPaths, ".asset");

		for (const fs::path &assetPath : assetPaths)
		{
			AssetPath ap{assetPath};
			AssetMetadata metadata;
			AssetFactoryManager::LoadMetadataHeader(ap, metadata);
			assets.emplace(ap, AssetData{ap, metadata});
		}
	}

#if EDITOR_ONLY
	void AssetManager::ImportEngineAssets()
	{
		std::vector<fs::path> assetPaths;
		FileHelper::GetFilesFromDirectory(engineImportPath, assetPaths);

		for (const fs::path &assetPath : assetPaths)
		{
			const std::string extension = assetPath.extension().string();
			if (AssetFactoryManager::SupportsExtension(extension))
			{
				ImportAsset(assetPath.string(), engineImportDestination);
				fs::remove(assetPath);
			}
		}
	}

	uint32_t AssetManager::GetExtensionId(const AssetPath &path)
	{
		const AssetManager &instance = AssetManager::Get();
		const auto it = instance.assets.find(path);
		if (it != instance.assets.end())
		{
			return it->second.metadata.type;
		}
		return 0;
	}

	void AssetManager::CreateAsset(const std::string &currentFolderPath, uint32_t type)
	{
		AssetManager &instance = AssetManager::Get();

		uint32_t index = 1;
		std::ostringstream oss;
		oss << currentFolderPath << "/NewAsset_" << index << ".asset";
		std::string filePath = oss.str();
		oss.str("");

		AssetPath path{filePath};
		while (instance.assets.contains(path))
		{
			++index;
			oss << currentFolderPath << "/NewAsset_" << index << ".asset";
			filePath = oss.str();
			path = AssetPath{filePath};
			oss.str("");
		}

		auto func = [&instance, path](const AssetData &data)
		{
			instance.assets.emplace(path, data);
		};

		AssetFactoryManager::CreateAssetFactory(path, type, func);
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
#endif
}