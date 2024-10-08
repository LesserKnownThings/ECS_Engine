#pragma once

#include "glm/glm.hpp"
#include "SDL/SDL_events.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "UI/EngineWindow.h"

#include <filesystem>
#include <functional>
#include <string>

namespace LKT
{
    class Asset;
    class Camera;
    class RenderBufferObject;

    struct AssetPath;

    constexpr float ASSET_VIEWER_ASPECT_RATIO = (16.0f / 9.0f);

    class AssetViewerWindow : public EngineWindow
    {
    public:
        AssetViewerWindow(LazyAssetPtr<Asset> &inAsset);

        void Initialize(const std::string &inName) override {}
        virtual void Initialize();

        virtual void Uninitialize() override;

        const AssetPath &GetAssetPath() const;

        bool IsShowing() const { return isShowing; }

    protected:
        void Render();
        void ProcessMouseMovement(float deltaTime);

        virtual void HandleWindowResized();

        virtual void HandleZoom(float amount);
        void HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent);
        void HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent);

        void RenderAsset();
        virtual void RenderAssetContent() = 0;

        // Available size for the asset rendering, this is used when you need to render a specific size in the window
        float availableX = 0;
        float availableY = 0;

        LazyAssetPtr<Asset> asset;
        Camera *camera = nullptr;
        RenderBufferObject *renderBuffer = nullptr;
        glm::mat4 assetModel = glm::mat4(1.f);

        bool isMouseMoving = false;

    private:
        int32_t previousX = 0;
        int32_t previousY = 0;

        friend class EditorMainWindow;
    };
}