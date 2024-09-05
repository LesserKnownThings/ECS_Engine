#pragma once

#include "UI/EngineWindow.h"
#include "SDL/SDL_events.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace LKT
{
    struct FileBrowserSettings
    {
        float columnX = 100.0f;
        float columnY = 100.0f;

        float textureX = 65.0f;
        float textureY = 65.0f;

        float paddingX = 10.0f;
        float paddingY = 10.0f;

        float marginX = 6.0f;
        float marginY = 6.0f;
    };

    class AssetManagerWindow : public EngineWindow
    {
    protected:
        void Initialize(const std::string &windowName) override;
        void Uninitialize() override;
        void RenderContent() override;

    private:
        void BrowseDirectory(const std::string &contentFolder);

        void BrowseDirectory();

        bool DrawTree(const std::filesystem::path &path);
        void DrawLeaf(const std::filesystem::path &path);
        void DrawSelectableItem(const std::filesystem::directory_entry &entry);

        void HandleMouseButtonReleased(const SDL_MouseButtonEvent& mouseEvent);

        const std::string defaultDirectory = "Content";
        std::string currentSelection;
        std::string innerSelection;

        std::string parentSelection;
        bool forceExpand = false;

        FileBrowserSettings fileBrowserSettings;
    };

    REGISTER_WINDOW(AssetManagerWindow, "Content Browser")
}