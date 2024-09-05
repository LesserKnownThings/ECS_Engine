#pragma once

#include "Asset.h"
#include "AssetPath.h"

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace LKT
{
	class AssetViewerWindow;

	constexpr uint32_t FOLDER = 0x0001;

	constexpr uint32_t TEXTURE = 0x0002;
	constexpr uint32_t MESH = 0x0004;

	using FactoryFunc = std::function<Asset *(const AssetPath &path)>;
	using FactoryUIFunc = std::function<AssetViewerWindow *(const AssetPath &path)>;

	struct AssetFactoryData
	{
		std::unordered_set<std::string> extensions;
		FactoryFunc func;
		FactoryUIFunc uiFunc;
		uint32_t id;
	};

	class AssetFactory
	{
	public:
		static AssetFactory &Get();

		static bool SupportsExtension(const std::string &extension);
		static uint32_t GetAssetExtensionId(const std::string &extension);
		static bool GetFactoryData(const std::string &extension, AssetFactoryData &outFactoryData);

		template <typename T>
		T *LoadAsset(const AssetPath &path);

	private:
		AssetFactory();

		void LoadFactories();
		void LoadSupportedExtensions();

		std::unordered_map<uint32_t, AssetFactoryData> factories;
		std::unordered_map<std::string, uint32_t> supportedExtensions;
	};

	template <typename T>
	inline T *AssetFactory::LoadAsset(const AssetPath &path)
	{
		const auto supportedExtensionIt = supportedExtensions.find(path.extension);

		if (supportedExtensionIt != supportedExtensions.end())
		{
			const auto it = factories.find(supportedExtensionIt->second);

			if (it != factories.end())
			{
				return reinterpret_cast<T *>(it->second.func(path));
			}
		}

		return nullptr;
	}
}