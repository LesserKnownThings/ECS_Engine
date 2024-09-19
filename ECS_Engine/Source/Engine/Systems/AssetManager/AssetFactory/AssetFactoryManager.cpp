#include "AssetFactoryManager.h"
#include "AssetFactory.h"
#include "Systems/AssetManager/AssetData.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/AssetPath.h"

#include <iostream>
#include <fstream>
#include "AssetFactoryManager.h"

namespace LKT
{
	AssetFactoryManager::AssetFactoryManager()
	{
	}

	AssetFactoryManager &AssetFactoryManager::Get()
	{
		static AssetFactoryManager instance;
		return instance;
	}

	void AssetFactoryManager::RegisterFactory(AssetFactory *newFactory, uint32_t factoryId)
	{
		factories.emplace(factoryId, newFactory);
	}

	bool AssetFactoryManager::SupportsExtension(const std::string &extension)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		return instance.supportedExtensions.find(extension) != instance.supportedExtensions.end();
	}

	uint32_t AssetFactoryManager::GetAssetExtensionId(const std::string &extension)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		const auto it = instance.supportedExtensions.find(extension);
		if (it != instance.supportedExtensions.end())
		{
			return it->second;
		}
		return 0;
	}

	void AssetFactoryManager::LoadMetadataHeader(const AssetPath &path, AssetMetadata &outMetadata)
	{
		std::ifstream stream(path.fullPath);
		if (stream.is_open())
		{
			stream.read(reinterpret_cast<char *>(&outMetadata.type), sizeof(uint32_t));
			stream.read(reinterpret_cast<char *>(&outMetadata.pathSize), sizeof(int32_t));

			outMetadata.path.resize(outMetadata.pathSize);
			stream.read(reinterpret_cast<char *>(&outMetadata.path[0]), outMetadata.pathSize);
		}
		stream.close();
	}

	Asset *AssetFactoryManager::LoadAssetFactory(const AssetMetadata &metadata)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		const auto it = instance.factories.find(metadata.type);
		if (it != instance.factories.end())
		{
			return it->second->LoadAsset(metadata);
		}
		return nullptr;
	}

	AssetViewerWindow *AssetFactoryManager::LoadAssetViewerFactory(const AssetMetadata &metadata, const AssetPath &path)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		const auto it = instance.factories.find(metadata.type);
		if (it != instance.factories.end())
		{
			return it->second->LoadAssetViewerWindow(path);
		}
		return nullptr;
	}

	bool AssetFactoryManager::ImportAssetFactory(const std::string &path,
												 const std::string &currentFolder,
												 std::vector<AssetData> &outData)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		fs::path p(path);

		const auto typeIt = instance.supportedExtensions.find(p.extension().string());
		if (typeIt != instance.supportedExtensions.end())
		{
			const auto it = instance.factories.find(typeIt->second);
			if (it != instance.factories.end())
			{
				it->second->CreateAsset(path, currentFolder, outData);
				return true;
			}
		}
		return false;
	}

	bool AssetFactoryManager::CreateAssetFactory(const AssetPath &fullPath, uint32_t type, std::function<void(const AssetData &)> func)
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		const auto it = instance.factories.find(type);
		if (it != instance.factories.end())
		{
			it->second->CreateAsset(fullPath, func);
			return true;
		}
		return false;
	}

	std::string AssetFactoryManager::GetExtensions()
	{
		return AssetFactoryManager::Get().extensionsString;
	}

	const std::unordered_map<uint32_t, AssetFactory *> &AssetFactoryManager::GetFactories()
	{
		const AssetFactoryManager &instance = AssetFactoryManager::Get();
		return instance.factories;
	}

	void AssetFactoryManager::LoadSupportedExtensions()
	{
		AssetFactoryManager &instance = AssetFactoryManager::Get();

		for (const auto &it : instance.factories)
		{
			for (const std::string &extension : it.second->GetSupportedExtensions())
			{
				instance.supportedExtensions.emplace(extension, it.second->GetType());

				if (extension != ".asset")
				{
					instance.extensionsString += "*" + extension + " ";
				}
			}
		}
	}
}
