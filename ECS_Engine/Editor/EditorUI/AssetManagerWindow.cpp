#include "AssetManagerWindow.h"
#include "Assets/Texture.h"
#include "imgui/imgui.h"
#include "Systems/AssetManager/AssetFactory/AssetFactory.h"
#include "Systems/AssetManager/AssetFactory/AssetFactoryManager.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "UI/UIManager.h"

#include "portable-file-dialogs.h"
#include <filesystem>
#include <thread>
#include <future>

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

        LoadTextureDisplayAssets();
        InputManagerSystem::Get().onMouseReleased.Bind(this, &AssetManagerWindow::HandleMouseButtonReleased);

        windowFlags |= ImGuiWindowFlags_MenuBar;
    }

    void AssetManagerWindow::Uninitialize()
    {
        InputManagerSystem::Get().onMouseReleased.Clear(this);

        EngineWindow::Uninitialize();
    }

    void AssetManagerWindow::LoadTextureDisplayAssets()
    {
        // Folder
        auto folderFunc = [this](const LazyAssetPtr<Asset> &asset)
        {
            textureDisplayAssets.emplace(FOLDER, asset.StrongRef());
        };
        AssetManager::GetAsset(AssetPath("Data/Content/Assets/Folder.asset"), folderFunc);

        // Mesh
        auto meshFunc = [this](const LazyAssetPtr<Asset> &asset)
        {
            textureDisplayAssets.emplace(MESH, asset.StrongRef());
        };
        AssetManager::GetAsset(AssetPath("Data/Content/Assets/Mesh.asset"), meshFunc);

        // Texture
        auto textureFunc = [this](const LazyAssetPtr<Asset> &asset)
        {
            textureDisplayAssets.emplace(TEXTURE, asset.StrongRef());
        };
        AssetManager::GetAsset(AssetPath("Data/Content/Assets/Texture.asset"), textureFunc);

        // Particle system
        auto particleFunc = [this](const LazyAssetPtr<Asset> &asset)
        {
            textureDisplayAssets.emplace(PARTICLESYSTEM, asset.StrongRef());
        };
        AssetManager::GetAsset(AssetPath("Data/Content/Assets/Particles.asset"), particleFunc);
    }

    void AssetManagerWindow::RenderContent()
    {
        DrawCommands();
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

            if (LKT::DirectoryIsEmpty(engineDefaultDirectory))
            {
                DrawLeaf(engineDefaultDirectory);
            }
            else if (DrawTree(fs::path(engineDefaultDirectory)))
            {
                BrowseDirectory(engineDefaultDirectory);
                ImGui::TreePop();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::BeginChild("##box", ImVec2(0.f, 0.f));
            BrowseDirectory();
            ImGui::EndChild();
            ImGui::EndTable();
        }
    }

    void AssetManagerWindow::DrawCommands()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::Button("Import"))
            {
                std::string ext = AssetFactoryManager::GetExtensions();
                pfd::open_file selection = pfd::open_file("Select a file", ".",
                                                          {"Files", ext},
                                                          pfd::opt::none);

                while (!selection.ready(1000)) // Maybe add a log or something
                    ;

                const std::vector<std::string> results = selection.result();
                if (results.size() > 0)
                {
                    const std::string path = results[0];
                    AssetManager::ImportAsset(path, currentSelection);
                }
            }

            if (ImGui::BeginMenu("Create"))
            {
                for (const auto &it : AssetFactoryManager::GetFactories())
                {
                    if (it.second->CanCreateFromMenu())
                    {
                        if (ImGui::MenuItem(it.second->GetMenuDisplayName().c_str()))
                        {
                            AssetManager::Get().CreateAsset(currentSelection, it.second->GetType());
                        }
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void AssetManagerWindow::HandleFileDialogClosed(const std::string &filePath)
    {
        InputManagerSystem::Get().UnblockInput();
        std::cout << filePath << std::endl;
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
                                 (fileBrowserSettings.columnX + fileBrowserSettings.cellPaddingX * 2);
            maxColumns = std::max(2, maxColumns);

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(fileBrowserSettings.cellPaddingX, fileBrowserSettings.cellPaddingY));
            if (ImGui::BeginTable("Table", maxColumns))
            {
                for (int32_t i = 0; i < maxColumns; ++i)
                {
                    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, (fileBrowserSettings.columnX));
                }

                int32_t columnIndex = 0;
                ImGui::TableNextRow();
                for (const auto &entry : fs::directory_iterator(currentSelection))
                {
                    // Only support .asset
                    if (!entry.is_directory() && entry.path().extension().string() != ".asset")
                    {
                        continue;
                    }

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

        const float selectableAreaX = fileBrowserSettings.columnX;
        const float selectableAreaY = fileBrowserSettings.columnY;

        ImVec2 newPos = ImGui::GetCursorScreenPos();
        ImVec4 clipRect(newPos.x, newPos.y, newPos.x + fileBrowserSettings.columnX, newPos.y + fileBrowserSettings.columnY);

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

        currentPos.x += (fileBrowserSettings.columnX - fileBrowserSettings.textureX) / 2;
        currentPos.y -= fileBrowserSettings.columnY;

        ImGui::SetCursorPos(currentPos);

        // Need to use string and then c_str because c_str expires before ImGui can calculate the size
        const std::string currentTextC = entry.path().stem().string();
        const float textSize = ImGui::CalcTextSize(currentTextC.c_str()).x;

        if (!entry.is_directory())
        {
            AssetPath assetPath{entry.path()};

            const uint32_t extensionId = AssetManager::GetExtensionId(assetPath);

            uint32_t textureID = 0;
            if (extensionId != 0)
            {
                const auto &textureAssetIt = textureDisplayAssets.find(extensionId);
                if (textureAssetIt != textureDisplayAssets.end())
                {
                    if (const Texture *textAsset = textureAssetIt->second.Get<Texture>())
                    {
                        textureID = textAsset->GetTextureID();
                    }
                }
            }

            ImTextureID texture = reinterpret_cast<ImTextureID>(textureID);
            ImGui::Image(texture, ImVec2(fileBrowserSettings.textureX, fileBrowserSettings.textureY));

            currentPos = ImGui::GetCursorPos();
            currentPos.y += fileBrowserSettings.textOffset;

            if (textSize < fileBrowserSettings.columnX)
            {
                const float offsetX = (fileBrowserSettings.columnX - textSize) * 0.5f;
                currentPos.x += offsetX;
            }

            ImGui::SetCursorPos(currentPos);

            newPos = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), newPos, IM_COL32_BLACK, entry.path().stem().c_str(), nullptr, fileBrowserSettings.columnX, &clipRect);

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isSelected)
            {
                UIManager::Get().RequestAssetViewer(assetPath);
            }
        }
        else
        {
            uint32_t textureID = 0;
            const auto folderAssetIt = textureDisplayAssets.find(FOLDER);
            if (folderAssetIt != textureDisplayAssets.end())
            {
                if (const Texture *textAsset = folderAssetIt->second.Get<Texture>())
                {
                    textureID = textAsset->GetTextureID();
                }
            }

            ImTextureID texture = reinterpret_cast<ImTextureID>(textureID);

            ImGui::Image(texture, ImVec2(fileBrowserSettings.textureX, fileBrowserSettings.textureY));

            currentPos = ImGui::GetCursorPos();
            currentPos.y += fileBrowserSettings.textOffset;

            if (textSize < fileBrowserSettings.columnX)
            {
                const float offsetX = (fileBrowserSettings.columnX - textSize) * 0.5f;
                currentPos.x += offsetX;
            }

            ImGui::SetCursorPos(currentPos);

            newPos = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), newPos, IM_COL32_BLACK, entry.path().stem().c_str(), nullptr, fileBrowserSettings.columnX, &clipRect);

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