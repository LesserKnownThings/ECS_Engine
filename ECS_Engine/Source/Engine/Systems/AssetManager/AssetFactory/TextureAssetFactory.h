#pragma once

#include "AssetFactory.h"

namespace LKT
{
    class TextureAssetFactory : public AssetFactory
    {
    public:
        void SetExtensions() override;
        uint32_t GetType() override;

    protected:
        void RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath);

        Asset *AssetFunc() override;

#if EDITOR_ONLY
        AssetViewerWindow *AssetViewerFunc(const AssetPath &path) override;
#endif
    };

    REGISTER_FACTORY(TextureAssetFactory, TEXTURE)
}