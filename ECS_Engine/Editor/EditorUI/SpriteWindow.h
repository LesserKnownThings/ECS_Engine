#pragma once

#include "AssetViewerWindow.h"

namespace LKT
{
    class SpriteWindow : public AssetViewerWindow
    {
    public:
        SpriteWindow(LazyAssetPtr<Asset> &inAsset);

    protected:
        void RenderContent() override;
        void RenderAssetContent() override;
    };
}