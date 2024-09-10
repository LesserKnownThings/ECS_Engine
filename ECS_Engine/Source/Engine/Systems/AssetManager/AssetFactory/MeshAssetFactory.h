#pragma once

#include "AssetFactory.h"

namespace LKT
{
    struct MeshData;

    class MeshAssetFactory : public AssetFactory
    {
    public:
        void CreateAsset(const std::string &importedAssetPath,
                         const std::string &currentFolderPath,
                         std::vector<AssetData> &outData) override;

        void SetExtensions() override;
        uint32_t GetType() override;

    protected:
        // This is empty because the mesh factory can create multiple assets
        virtual void RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath) override {}

        Asset *AssetFunc() override;
#if EDITOR_ONLY
        AssetViewerWindow *AssetViewerFunc(const AssetPath &path) override;
#endif
    };

    REGISTER_FACTORY(MeshAssetFactory, MESH)
}