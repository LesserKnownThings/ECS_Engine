#include "MeshAssetFactory.h"
#include "Assets/Mesh.h"
#include "Systems/AssetManager/AssetData.h"
#include "Systems/AssetManager/AssetMetadata.h"
#include "Systems/AssetManager/AssetPath.h"
#include "Systems/MeshLoadingSystem.h"

#if EDITOR_ONLY
#include "EditorUI/MeshAssetViewer.h"
#endif

#include <vector>

namespace LKT
{
    /**
     * The mesh factory is a bit different from other factories because it can create multiple assets
     * when importing 1 mesh that's because some meshes are separated in sub meshes and the engine
     * treats every submesh as an individual mesh.
     */
    void MeshAssetFactory::CreateAsset(const std::string &importedAssetPath,
                                       const std::string &currentFolderPath,
                                       std::vector<AssetData> &outData)
    {
        std::vector<MeshData> meshData;
        if (MeshLoadingSystem::ImportMesh(importedAssetPath, meshData))
        {
            for (const MeshData &mesh : meshData)
            {
                const std::string newAssetPath = currentFolderPath + "/" + mesh.name + ".asset";
                std::ofstream stream(newAssetPath, std::ios::binary);

                AssetPath assetPath{newAssetPath};
                AssetMetadata metadata;

                if (stream.is_open())
                {
                    CreateAssetMetadata(assetPath, stream, metadata);
                    mesh.Serialize(stream);
                }

                stream.close();

                outData.push_back(AssetData{assetPath, metadata});
            }
        }
    }

    void MeshAssetFactory::SetExtensions()
    {
        supportedExtensions = {".obj", ".fbx"};
    }

    uint32_t MeshAssetFactory::GetType()
    {
        return MESH;
    }

    Asset *MeshAssetFactory::AssetFunc()
    {
        return new Mesh();
    }

#if EDITOR_ONLY
    AssetViewerWindow *MeshAssetFactory::AssetViewerFunc(const AssetPath &path)
    {
        AssetViewerWindow *newWindow = nullptr;

        auto func = [&newWindow](const LazyAssetPtr<Asset> &asset)
        {
            newWindow = new MeshAssetViewer(asset.StrongRef());
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