#pragma once

#include "UI/EngineWindow.h"
#include "Systems/AssetManager/LazyAssetPtr.h"

#include <filesystem>
#include <functional>
#include <string>

namespace LKT
{
    class Asset;
    class Camera;

    struct AssetPath;

    constexpr float ASSET_VIEWER_ASPECT_RATIO = (16.0f / 9.0f);

    class AssetViewerWindow : public EngineWindow
    {
    public:
        AssetViewerWindow(LazyAssetPtr<Asset> &inAsset);

        void Initialize(const std::string &inName) override {}
        virtual void Initialize();

        virtual void Uninitialize() override;
        void Focus();

        const AssetPath &GetAssetPath() const;

        bool IsShowing() const { return isShowing; }

    protected:
        void Render();
        virtual void HandleWindowResized();

        virtual void RenderAsset() = 0;

        // Available size for the asset rendering, this is used when you need to render a specific size in the window
        float availableX = 0;
        float availableY = 0;

        std::function<void(const EngineWindow *, float, float)> renderBufferWindowResizeCallback;

        LazyAssetPtr<Asset> asset;
        Camera *camera = nullptr;

    private:
        int32_t previousX = 0;
        int32_t previousY = 0;

        friend class EditorMainWindow;
    };
}