#pragma once

#include "AssetPath.h"
#include "LazyAssetPtr.h"

#include <cstdint>
#include <functional>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace LKT
{
	class Asset;

	using AssetGetFunction = std::function<void(LazyAssetPtr<Asset>& asset)>;

	struct AssetTextureStorage
	{
		uint32_t id;
		std::string path;
	};

	class AssetManager
	{
	public:
		static AssetManager &Get();

		void GetAsset(const AssetPath &inPath, AssetGetFunction func);
		uint32_t GetAssetTexture(const std::string &extension) const;
		uint32_t GetFolderTexture() const;

	private:
		void BuildAssetRegistry();
		void LoadAssetViewTextures();

		const std::string contentFolder = "Content";

		std::unordered_map<AssetPath, LazyAssetPtr<Asset>, AssetPathHash> assets;

		std::unordered_map<uint32_t, uint32_t> assetViewTextures;

		std::vector<AssetTextureStorage> textureStorage;

		friend class Engine;
	};
}