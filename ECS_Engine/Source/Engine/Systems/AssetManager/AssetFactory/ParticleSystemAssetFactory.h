#pragma once

#include "AssetFactory.h"

namespace LKT
{
    class ParticleSystemAssetFactory : public AssetFactory
    {
    public:
        void SetExtensions() override;
        uint32_t GetType() override;
        bool CanCreateFromMenu() const override;
        std::string GetMenuDisplayName() const override;

    protected:
        void RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath) override;

        Asset *AssetFunc() override;

#if EDITOR_ONLY
        AssetViewerWindow *AssetViewerFunc(const AssetPath &path) override;
#endif
    };

    REGISTER_FACTORY(ParticleSystemAssetFactory, PARTICLESYSTEM)
}