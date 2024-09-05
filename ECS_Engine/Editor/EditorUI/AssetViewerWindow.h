#pragma once

#include "UI/EngineWindow.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#include <filesystem>
#include <string>

namespace LKT
{
    class Asset;
    struct AssetPath;

    class AssetViewerWindow : public EngineWindow
    {
    public:
        AssetViewerWindow(LazyAssetPtr<Asset> &inAsset);

        void Initialize(const std::string &inName) override {}
        void Initialize();

        void Uninitialize() override;

        const AssetPath &GetAssetPath() const;

        bool IsShowing() const { return isShowing; }

    protected:
        void Render();

        LazyAssetPtr<Asset> asset;
        friend class EditorMainWindow;
    };
}