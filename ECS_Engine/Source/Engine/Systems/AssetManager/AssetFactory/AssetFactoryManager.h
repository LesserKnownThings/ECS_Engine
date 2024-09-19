#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace LKT
{
	class Asset;
	class AssetFactory;
	class AssetViewerWindow;

	struct AssetData;
	struct AssetMetadata;
	struct AssetPath;

	class AssetFactoryManager
	{
	public:
		static AssetFactoryManager &Get();

		static void LoadSupportedExtensions();
		static std::string GetExtensions();
		static const std::unordered_map<uint32_t, AssetFactory *> &GetFactories();

		static bool SupportsExtension(const std::string &extension);
		static uint32_t GetAssetExtensionId(const std::string &extension);

		static void LoadMetadataHeader(const AssetPath &path, AssetMetadata &outMetadata);
		static Asset *LoadAssetFactory(const AssetMetadata &metadata);
		static AssetViewerWindow *LoadAssetViewerFactory(const AssetMetadata &metadata, const AssetPath &path);
		static bool ImportAssetFactory(const std::string &path,
									   const std::string &currentFolder,
									   std::vector<AssetData> &outData);
		static bool CreateAssetFactory(const AssetPath &fullPath, uint32_t type, std::function<void(const AssetData &)> func);

		void RegisterFactory(AssetFactory *newFactory, uint32_t factoryId);

	private:
		AssetFactoryManager();

		std::string extensionsString;

		std::unordered_map<uint32_t, AssetFactory *> factories;
		std::unordered_map<std::string, uint32_t> supportedExtensions;
	};

#define REGISTER_FACTORY(factoryType, factoryId)                        \
	struct Factory##factoryType                                         \
	{                                                                   \
		Factory##factoryType()                                          \
		{                                                               \
			AssetFactoryManager &instance = AssetFactoryManager::Get(); \
			factoryType *newFactory = new factoryType();                \
			newFactory->SetType(factoryId);                             \
			newFactory->SetExtensions();                                \
			instance.RegisterFactory(newFactory, factoryId);            \
		}                                                               \
	};                                                                  \
	static Factory##factoryType Factory##factoryTypeRegister;
}