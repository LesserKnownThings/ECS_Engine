#pragma once

#include "UI/EngineWindow.h"
#include "SDL/SDL_events.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace LKT
{
    class Asset;
    template <typename T>
    class LazyAssetPtr;

    struct FileBrowserSettings
    {
        float columnX = 100.0f;
        float columnY = 100.0f;

        float textureX = 50.0f;
        float textureY = 50.0f;

        float textOffset = 5.0f;

        float cellPaddingX = 10.0f;
        float cellPaddingY = 10.0f;
    };

    class AssetManagerWindow : public EngineWindow
    {
    public:
        void Initialize(const std::string &windowName) override;
        void Uninitialize() override;

    protected:
        void RenderContent() override;

    private:
        void DrawCommands();
        void BrowseDirectory(const std::string &contentFolder);

        void BrowseDirectory();

        bool DrawTree(const std::filesystem::path &path);
        void DrawLeaf(const std::filesystem::path &path);
        void DrawSelectableItem(const std::filesystem::directory_entry &entry);

        void HandleMouseButtonReleased(const SDL_MouseButtonEvent &mouseEvent);
        void HandleFileDialogClosed(const std::string &filePath);

        void LoadTextureDisplayAssets();

        const std::string defaultDirectory = "Content";
        const std::string engineDefaultDirectory = "Data/Content/Assets";
        std::string currentSelection;
        std::string innerSelection;

        std::string parentSelection;
        bool forceExpand = false;

        std::unordered_map<uint32_t, LazyAssetPtr<Asset>> textureDisplayAssets;

        FileBrowserSettings fileBrowserSettings;
    };

    REGISTER_WINDOW(AssetManagerWindow, "Content Browser")
}