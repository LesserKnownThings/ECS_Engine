#include "AssetManagerWindow.h"
#include "imgui/imgui.h"
#include "Systems/AssetManager/AssetFactory.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "UI/UIManager.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace LKT
{
    namespace
    {
        static bool DirectoryIsEmpty(const std::string &inDirectory)
        {
            for (const auto &entry : fs::directory_iterator(inDirectory))
            {
                if (entry.is_directory())
                {
                    return false;
                }
            }

            return true;
        }
    }

    void AssetManagerWindow::Initialize(const std::string &windowName)
    {
        EngineWindow::Initialize(windowName);

        InputManagerSystem::Get().onMouseReleased.Bind(this, &AssetManagerWindow::HandleMouseButtonReleased);
    }

    void AssetManagerWindow::Uninitialize()
    {
        InputManagerSystem::Get().onMouseReleased.Clear(this);

        EngineWindow::Uninitialize();
    }

    void AssetManagerWindow::RenderContent()
    {
        if (ImGui::BeginTable("ContentBrowser", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("FolderTree", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("FileBrowser", ImGuiTableColumnFlags_None);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            const fs::path defaultDirectoryPath{defaultDirectory};

            if (LKT::DirectoryIsEmpty(defaultDirectoryPath))
            {
                DrawLeaf(defaultDirectoryPath);
            }
            else if (DrawTree(fs::path(defaultDirectory)))
            {
                BrowseDirectory(defaultDirectory);
                ImGui::TreePop();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::BeginChild("##box", ImVec2(0.f, 0.f));
            BrowseDirectory();
            ImGui::EndChild();

            ImGui::EndTable();
        }
    }

    void AssetManagerWindow::BrowseDirectory(const std::string &contentFolder)
    {
        for (const auto &entry : fs::directory_iterator(contentFolder))
        {
            const fs::path currentPath = entry.path();

            if (entry.is_directory())
            {
                if (LKT::DirectoryIsEmpty(currentPath))
                {
                    DrawLeaf(currentPath);
                }
                else if (DrawTree(currentPath))
                {
                    BrowseDirectory(currentPath.string());
                    ImGui::TreePop();
                }
            }
        }
    }

    bool AssetManagerWindow::DrawTree(const fs::path &path)
    {
        ImGuiTreeNodeFlags currentNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (currentSelection == path.string())
        {
            currentNodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        if (forceExpand && parentSelection == path.string())
        {
            ImGui::SetNextItemOpen(true);
        }

        const bool isOpen = ImGui::TreeNodeEx(path.stem().c_str(), currentNodeFlags);

        if (ImGui::IsItemClicked())
        {
            currentSelection = path.string();
        }

        return isOpen;
    }

    void AssetManagerWindow::DrawLeaf(const std::filesystem::path &path)
    {
        ImGui::Indent();
        const bool isSelected = currentSelection == path.string();
        if (ImGui::Selectable(path.stem().c_str(), isSelected, ImGuiSelectableFlags_None))
        {
            currentSelection = path.string();
        }
        ImGui::Unindent();
    }

    void AssetManagerWindow::BrowseDirectory()
    {
        if (fs::exists(currentSelection))
        {
            const int32_t availableRegion = ImGui::GetContentRegionAvail().x;
            int32_t maxColumns = availableRegion /
                                 (fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2 + fileBrowserSettings.paddingX * 2);
            maxColumns = std::max(2, maxColumns);

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(fileBrowserSettings.paddingX, fileBrowserSettings.paddingY));
            if (ImGui::BeginTable("Table", maxColumns))
            {
                for (int32_t i = 0; i < maxColumns; ++i)
                {
                    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, (fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2));
                }

                int32_t columnIndex = 0;
                ImGui::TableNextRow();
                for (const auto &entry : fs::directory_iterator(currentSelection))
                {
                    ImGui::TableSetColumnIndex(columnIndex);
                    DrawSelectableItem(entry);

                    columnIndex++;
                    if (columnIndex >= maxColumns)
                    {
                        columnIndex = 0;
                        ImGui::TableNextRow();
                    }
                }
                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        }
        else
        {
            currentSelection = defaultDirectory;
        }
    }

    void AssetManagerWindow::DrawSelectableItem(const fs::directory_entry &entry)
    {
        const std::string entryPath = entry.path().string();

        bool isSelected = entryPath == innerSelection;

        const std::string selectableName = entry.path().stem().string();
        const std::string selectableNameID = "##" + selectableName;

        const float selectableAreaX = fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2.0f;
        const float selectableAreaY = fileBrowserSettings.columnY + fileBrowserSettings.marginY * 2.0f;

        ImVec2 newPos = ImGui::GetCursorScreenPos();
        ImVec4 clipRect(newPos.x, newPos.y, newPos.x + fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2, newPos.y + fileBrowserSettings.columnY + fileBrowserSettings.marginY * 2);

        if (ImGui::Selectable(selectableNameID.c_str(),
                              isSelected,
                              ImGuiSelectableFlags_None,
                              ImVec2(selectableAreaX, selectableAreaY)))
        {
            innerSelection = entryPath;
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text(selectableName.c_str());
            ImGui::EndTooltip();
        }

        ImVec2 currentPos = ImGui::GetCursorPos();

        currentPos.x += fileBrowserSettings.textureX - fileBrowserSettings.marginX - fileBrowserSettings.textureX / 2.0f - fileBrowserSettings.marginX / 2.0f;
        currentPos.y -= fileBrowserSettings.columnY + fileBrowserSettings.marginY + fileBrowserSettings.paddingY;
        ImGui::SetCursorPos(currentPos);

        if (!entry.is_directory())
        {
            const uint32_t textureID = AssetManager::Get().GetAssetTexture(entry.path().extension().string());
            ImTextureID texture = reinterpret_cast<ImTextureID>(textureID);

            ImGui::Image(texture, ImVec2(fileBrowserSettings.textureX, fileBrowserSettings.textureY));

            currentPos = ImGui::GetCursorPos();
            currentPos.y += fileBrowserSettings.marginY;
            ImGui::SetCursorPos(currentPos);

            newPos = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), newPos, IM_COL32_BLACK, entry.path().stem().c_str(), nullptr, fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2, &clipRect);

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isSelected)
            {
                UIManager::Get().RequestAssetViewer(entry.path());
            }
        }
        else
        {
            const uint32_t textureID = AssetManager::Get().GetFolderTexture();
            ImTextureID texture = reinterpret_cast<ImTextureID>(textureID);

            ImGui::Image(texture, ImVec2(fileBrowserSettings.textureX, fileBrowserSettings.textureY));

            currentPos = ImGui::GetCursorPos();
            currentPos.y += fileBrowserSettings.marginY;
            ImGui::SetCursorPos(currentPos);

            newPos = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), newPos, IM_COL32_BLACK, entry.path().stem().c_str(), nullptr, fileBrowserSettings.columnX + fileBrowserSettings.marginX * 2, &clipRect);

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isSelected)
            {
                forceExpand = true;
                currentSelection = entryPath;
                parentSelection = entry.path().parent_path();
            }
        }
    }

    void AssetManagerWindow::HandleMouseButtonReleased(const SDL_MouseButtonEvent &mouseEvent)
    {
        innerSelection.clear();
    }
}