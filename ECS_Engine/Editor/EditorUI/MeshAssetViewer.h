#pragma once

#include "AssetViewerWindow.h"

namespace LKT
{
    class MeshAssetViewer : public AssetViewerWindow
    {
    public:
        MeshAssetViewer(LazyAssetPtr<Asset> &inAsset);

    protected:
        void RenderContent() override;
    };
}