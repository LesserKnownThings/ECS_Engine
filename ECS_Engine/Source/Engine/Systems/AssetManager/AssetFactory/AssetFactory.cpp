#include "AssetFactory.h"
#include "AssetFactoryManager.h"
#include "FileHelper.h"
#include "Systems/AssetManager/AssetData.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/AssetMetadata.h"
#include "Systems/AssetManager/AssetPath.h"

namespace LKT
{
    void AssetFactory::CreateAsset(const std::string &importedAssetPath,
                                   const std::string &currentFolderPath,
                                   std::vector<AssetData> &outData)
    {
        fs::path path(importedAssetPath);
        const std::string newAssetPath = currentFolderPath + "/" + path.stem().string() + ".asset";
        std::ofstream stream(newAssetPath, std::ios::binary);

        AssetPath assetPath{newAssetPath};
        AssetMetadata metadata; // Careful, this metadata doesn't have the buffer container, only the header

        if (stream.is_open())
        {
            CreateAssetMetadata(assetPath, importedAssetPath, stream, metadata);
            RegisterMetadataData(stream, importedAssetPath);
        }

        stream.close();

        outData = {AssetData{assetPath, metadata}};
    }

    Asset *AssetFactory::LoadAsset(const AssetMetadata &metadata)
    {
        Asset *asset = AssetFunc();
        asset->LoadAsset(LoadAssetBuffer(metadata));
        return asset;
    }

#if EDITOR_ONLY
    AssetViewerWindow *AssetFactory::LoadAssetViewerWindow(const AssetPath &path)
    {
        return AssetViewerFunc(path);
    }

    bool AssetFactory::HandleAssetViewer(const AssetPath &path, std::function<void(const LazyAssetPtr<Asset> &asset)> func)
    {
        return AssetManager::GetAsset(path, func);
    }
#endif

    void AssetFactory::CreateAssetMetadata(const AssetPath &path,
                                           const std::string &importedAssetPath,
                                           std::ofstream &outStream,
                                           AssetMetadata &outMetadata)
    {
        outMetadata.pathSize = path.fullPath.size();
        outMetadata.type = type;
        outMetadata.path = path.fullPath;

        outStream.write(reinterpret_cast<const char *>(&outMetadata.type), sizeof(uint32_t));
        outStream.write(reinterpret_cast<const char *>(&outMetadata.pathSize), sizeof(int32_t));
        outStream.write(reinterpret_cast<const char *>(&outMetadata.path[0]), outMetadata.pathSize);
    }

    void *AssetFactory::LoadAssetBuffer(const AssetMetadata &metadata)
    {
        // We're skipping the metadata header since we already have it
        const int32_t offset = sizeof(uint32_t) + sizeof(int32_t) + metadata.pathSize;
        void *buffer = nullptr;

        std::ifstream stream(metadata.path, std::ios::binary | std::ios::ate);
        if (stream.is_open())
        {
            buffer = FileHelper::GetBufferFromStream(stream, offset);
        }
        stream.close();

        return buffer;
    }
}