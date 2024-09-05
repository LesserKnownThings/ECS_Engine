#include "AssetViewerWindow.h"
#include "Systems/AssetManager/Asset.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

#include "imgui/imgui.h"

namespace LKT
{
    void AssetViewerWindow::Initialize()
    {
        asset.LoadAsset();

        name = asset.GetName() + "##" + GetID().ToString();

        TaskManagerSystem::Get().RegisterTask(this, &AssetViewerWindow::Render, INT32_MAX - 1, RENDER_HANDLE);
    }

    AssetViewerWindow::AssetViewerWindow(LazyAssetPtr<Asset> &inAsset)
        : asset(inAsset)
    {
        std::string check;
    }

    void AssetViewerWindow::Uninitialize()
    {
        TaskManagerSystem::Get().RemoveTask(this);
    }

    const AssetPath &AssetViewerWindow::GetAssetPath() const
    {
        return asset.GetPath();
    }

    void AssetViewerWindow::Render()
    {
        if (!isShowing)
        {
            UIManager::Get().onUniqueWindowClosed.Invoke(this);
            return;
        }

        if (ImGui::Begin(name.c_str(), &isShowing))
        {
            RenderContent();
        }

        ImGui::End();
    }
}