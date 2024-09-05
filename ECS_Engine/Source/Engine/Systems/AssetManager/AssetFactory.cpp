#include "AssetFactory.h"
#include "Assets/Texture.h"
#include "EditorUI/TextureAssetViewer.h"
#include "EditorUI/MeshAssetViewer.h"
#include "Systems/AssetManager/AssetManager.h"

namespace LKT
{
	AssetFactory &AssetFactory::Get()
	{
		static AssetFactory instance;
		return instance;
	}

	bool AssetFactory::SupportsExtension(const std::string &extension)
	{
		const AssetFactory &instance = AssetFactory::Get();
		const auto it = instance.supportedExtensions.find(extension);
		return it != instance.supportedExtensions.end();
	}

	uint32_t AssetFactory::GetAssetExtensionId(const std::string &extension)
	{
		const AssetFactory &instance = AssetFactory::Get();

		const auto it = instance.supportedExtensions.find(extension);

		if (it != instance.supportedExtensions.end())
		{
			return it->second;
		}

		return 0;
	}

	bool AssetFactory::GetFactoryData(const std::string &extension, AssetFactoryData &outFactoryData)
	{
		const AssetFactory &instance = AssetFactory::Get();

		const auto idIt = instance.supportedExtensions.find(extension);

		if (idIt != instance.supportedExtensions.end())
		{
			const auto it = instance.factories.find(idIt->second);

			if (it != instance.factories.end())
			{
				outFactoryData = it->second;
				return true;
			}
		}

		return false;
	}

	AssetFactory::AssetFactory()
	{
		LoadFactories();
		LoadSupportedExtensions();
	}

	void AssetFactory::LoadFactories()
	{
		auto textureFactory = [](const AssetPath &path) -> Asset *
		{
			Asset *asset = new Texture();
			asset->LoadAsset(path);
			return asset;
		};

		auto textureUIFactory = [](const AssetPath &path) -> AssetViewerWindow *
		{
			AssetViewerWindow *newWindow = nullptr;

			auto func = [&newWindow](LazyAssetPtr<Asset> &asset)
			{
				newWindow = new TextureAssetViewer(asset.StrongRef());
				newWindow->Initialize();
			};

			AssetManager::Get().GetAsset(path, func);
			return newWindow;
		};

		AssetFactoryData textureFd{
			{".png", ".jpg", ".jpeg"},
			textureFactory,
			textureUIFactory,
			TEXTURE};

		auto meshFactory = [](const AssetPath &path) -> Asset *
		{
			return nullptr;
		};

		auto meshUIFactory = [](const AssetPath &path) -> AssetViewerWindow *
		{
			return nullptr;
		};

		AssetFactoryData meshFd{
			{".fbx", ".obj"},
			meshFactory,
			meshUIFactory,
			MESH};

		factories = {{textureFd.id, textureFd}, {meshFd.id, meshFd}};
	}

	void AssetFactory::LoadSupportedExtensions()
	{
		for (const auto &it : factories)
		{
			for (const std::string &extension : it.second.extensions)
			{
				supportedExtensions.emplace(extension, it.second.id);
			}
		}
	}
}
