#pragma once

#include "AssetMetadata.h"
#include "AssetPath.h"
#include "BuildMacros.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace LKT
{
	class Asset;
	template <typename T>
	class LazyAssetPtr;
	class AssetViewerWindow;

	struct AssetData;

	class AssetManager
	{
	public:
		static AssetManager &Get();

		static bool GetAsset(const AssetPath &path, std::function<void(const LazyAssetPtr<Asset> &asset)> assetProcessFunc);
		static bool GetAsset(const AssetPath &path, LazyAssetPtr<Asset> &outAsset);
		static Asset *LoadAsset(const AssetPath &path);

#if EDITOR_ONLY
		static uint32_t GetExtensionId(const AssetPath &path);
		static void CreateAsset(const std::string &currentFolderPath, uint32_t type);
		static void ImportAsset(const std::string &path, const std::string &currentFolder);

		static AssetViewerWindow *LoadAssetViewer(const AssetPath &path);
#endif

	private:
		void Initialize();
		void BuildRegistries();
		void BuildAssetRegistry();
		void BuildEngineAssetRegistry();
		void BuildRegistry(const std::string &path);

		const std::string contentFolder = "Content";

		/**
		 * The asset manager does not load the assets, it only keeps a lazy asset
		 * The factory manager loads the assets when required, but if you decide
		 * to do something manual, you will need to make sure to load the asset
		 * before using it.
		 */
		std::unordered_map<AssetPath, AssetData, AssetPathHash> assets;

		const std::string engineFolder = "Data/Content/Assets";

#if EDITOR_ONLY
		void ImportEngineAssets();

		const std::string engineImportPath = "Data/Import";
		const std::string engineImportDestination = "Data/Content/Assets";
#endif

		friend class Engine;
	};
}