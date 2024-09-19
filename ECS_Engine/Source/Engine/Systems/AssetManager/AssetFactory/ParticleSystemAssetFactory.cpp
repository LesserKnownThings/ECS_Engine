#include "ParticleSystemAssetFactory.h"
#include "Assets/ParticleSystemAsset.h"
#include "Systems/ParticleSystem/ParticleSystem.h"

#if EDITOR_ONLY
#include "EditorUI/ParticleSystemAssetViewer.h"
#endif

namespace LKT
{
    void ParticleSystemAssetFactory::SetExtensions()
    {
        // No additional extension support to add
    }

    uint32_t ParticleSystemAssetFactory::GetType()
    {
        return PARTICLESYSTEM;
    }

    bool ParticleSystemAssetFactory::CanCreateFromMenu() const
    {
        return true;
    }

    std::string ParticleSystemAssetFactory::GetMenuDisplayName() const
    {
        return "Particle System";
    }

    void ParticleSystemAssetFactory::RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath)
    {
        ParticleSystem::SerializeInitialData(stream);
    }

    Asset *ParticleSystemAssetFactory::AssetFunc()
    {
        return new ParticleSystemAsset();
    }

#if EDITOR_ONLY
    AssetViewerWindow *ParticleSystemAssetFactory::AssetViewerFunc(const AssetPath &path)
    {
        AssetViewerWindow *newWindow = nullptr;

        auto func = [&newWindow](const LazyAssetPtr<Asset> &asset)
        {
            newWindow = new ParticleSystemAssetViewer(asset.StrongRef());
            newWindow->Initialize();
        };

        if (HandleAssetViewer(path, func))
        {
            return newWindow;
        }

        return nullptr;
    }
#endif
}