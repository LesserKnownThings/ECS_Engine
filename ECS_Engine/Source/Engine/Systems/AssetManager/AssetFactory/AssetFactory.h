#pragma once

#include "AssetFactoryManager.h"
#include "BuildMacros.h"
#include "Systems/AssetManager/Asset.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#if EDITOR_ONLY
#include "EditorUI/AssetViewerWindow.h"
#endif

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace LKT
{
    namespace fs = std::filesystem;

    class Asset;
    class AssetViewerWindow;

    struct AssetData;
    struct AssetMetadata;
    struct AssetPath;

    enum AssetType : uint8_t
    {
        FOLDER,
        TEXTURE,
        MESH,
        PARTICLESYSTEM
    };

    class AssetFactory
    {
    public:
        virtual void SetExtensions() = 0;
        virtual uint32_t GetType() = 0;
        virtual bool CanCreateFromMenu() const { return false; }
        virtual std::string GetMenuDisplayName() const { return ""; }

        void SetType(uint32_t inType) { type = inType; }

        /**
         * This creates an asset based on the factory. The reason this has multiple assets
         * as an out is the fact that some factories can create multiple assets with
         * one import, like the mesh factory.
         */
        virtual void CreateAsset(const std::string &importedAssetPath,
                                 const std::string &currentFolderPath,
                                 std::vector<AssetData> &outData);
        virtual void CreateAsset(const AssetPath &path, std::function<void(const AssetData &)> func);

        Asset *LoadAsset(const AssetMetadata &metadata);

#if EDITOR_ONLY
        AssetViewerWindow *LoadAssetViewerWindow(const AssetPath &path);
#endif

        const std::vector<std::string> &GetSupportedExtensions() const
        {
            return supportedExtensions;
        }

    protected:
        void CreateAssetMetadata(const AssetPath &path,
                                 std::ofstream &outStream,
                                 AssetMetadata &outMetadata);
        void *LoadAssetBuffer(const AssetMetadata &metadata);

        virtual void RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath) = 0;

        virtual Asset *AssetFunc() = 0;
#if EDITOR_ONLY
        virtual AssetViewerWindow *AssetViewerFunc(const AssetPath &path) = 0;

        /**
         * You're probably asking yourself why this? Well since LazyPtr can't be created without a path
         * We need to pass the LazyPtr directly to the window creation function as a strong ref
         */
        bool HandleAssetViewer(const AssetPath &path, std::function<void(const LazyAssetPtr<Asset> &asset)> func);
#endif

        std::vector<std::string> supportedExtensions;
        uint32_t type;
    };
}