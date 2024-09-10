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

	struct AssetTextureStorage
	{
		uint32_t id;
		std::string path;
	};

	/**
	 * If you look at both folder for assets Data (Engine) and Content (Game) you will find the structure a bit weird
	 * Let me explain how exactly that works:
	 *
	 * The Data(Engine):
	 * 1. It has a Content folder which will be loaded as assets when the editor starts
	 * 2. It has a shader folder that contains all the engine shaders
	 * 3. It has a texture folder that contains textures that will not be loaded as assets, but as textures.
	 * These are used by the Content Browsers to display assets. The reason I'm not loading them as
	 * LazyAssets is because there's not a lot of textures and in most cases they will always be on screen.
	 * So I don't want to create/delete a texture every time I open a new folder.
	 *
	 * There's also an exception for the Texture folder. The assets
	 *
	 * The Content (Game)
	 * Everything in this folder and subfolders will be loaded in the asset manager in the editor. I'm saying in the editor.
	 * The reason I'm saying in the editor is because the build system will only move the assets that are referenced in the
	 * folder so when the game is launched (no editor) everything will be read from the Content folder, but it won't be the
	 * same assets as the editor.
	 */
	class AssetManager
	{
	public:
		static AssetManager &Get();

		static bool GetAsset(const AssetPath &path, std::function<void(const LazyAssetPtr<Asset> &asset)> assetProcessFunc);
		static Asset *LoadAsset(const AssetPath &path);

#if EDITOR_ONLY
		static void CreateAsset(const std::string &currentFolderPath, uint32_t type);
		static void ImportAsset(const std::string &path, const std::string &currentFolder);

		static AssetViewerWindow *LoadAssetViewer(const AssetPath &path);
		static uint32_t GetAssetTexture(const AssetPath &path);
		static uint32_t GetFolderTexture();
#endif

	private:
		void BuildAssetRegistry();

		const std::string contentFolder = "Content";
		std::unordered_map<AssetPath, AssetData, AssetPathHash> assets;

#if EDITOR_ONLY
		void LoadAssetViewTextures();

		std::unordered_map<uint32_t, uint32_t> assetViewTextures;
		std::vector<AssetTextureStorage> textureStorage;
#endif

		friend class Engine;
	};
}