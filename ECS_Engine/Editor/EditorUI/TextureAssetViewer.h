#pragma once

#include "AssetViewerWindow.h"

namespace LKT
{
    class TextureAssetViewer : public AssetViewerWindow
    {
    public:
        TextureAssetViewer(LazyAssetPtr<Asset> &inAsset);

    protected:
        void RenderContent() override;
        void RenderAssetContent() override {}
    };
}