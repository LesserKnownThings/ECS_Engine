#include "TextureAssetFactory.h"
#include "Assets/Texture.h"
#include "GraphicHelpers.h"

#if EDITOR_ONLY
#include "EditorUI/TextureAssetViewer.h"
#endif

namespace LKT
{
    void TextureAssetFactory::SetExtensions()
    {
        supportedExtensions = {".png", ".jpg", ".jpeg", ".bmp"};
    }

    uint32_t TextureAssetFactory::GetType()
    {
        return TEXTURE;
    }

    void TextureAssetFactory::RegisterMetadataData(std::ofstream &stream, const std::string &importedAssetPath)
    {
        int32_t width, height, components;
        unsigned char *data;
        if (GraphicHelpers::LoadTexture(importedAssetPath, width, height, components, data))
        {
            stream.write(reinterpret_cast<const char *>(&width), sizeof(int32_t));
            stream.write(reinterpret_cast<const char *>(&height), sizeof(int32_t));
            stream.write(reinterpret_cast<const char *>(&components), sizeof(int32_t));

            const int32_t bufferSize = width * height * components;
            stream.write(reinterpret_cast<const char *>(data), bufferSize);
        }
    }

    Asset *TextureAssetFactory::AssetFunc()
    {
        return new Texture();
    }

#if EDITOR_ONLY
    AssetViewerWindow *TextureAssetFactory::AssetViewerFunc(const AssetPath &path)
    {
        AssetViewerWindow *newWindow = nullptr;

        auto func = [&newWindow](const LazyAssetPtr<Asset> &asset)
        {
            newWindow = new TextureAssetViewer(asset.StrongRef());
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