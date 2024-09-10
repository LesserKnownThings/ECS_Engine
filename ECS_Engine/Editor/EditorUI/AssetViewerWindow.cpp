#include "AssetViewerWindow.h"
#include "Camera.h"
#include "Systems/AssetManager/Asset.h"
#include "Systems/RenderBufferSystem.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

#include "imgui/imgui.h"

namespace LKT
{
    AssetViewerWindow::AssetViewerWindow(LazyAssetPtr<Asset> &inAsset)
        : asset(inAsset)
    {
        windowCustomFlags = WINDOW_SUPPORT_RENDER_BUFFER | WINDOW_SUPPORT_CAMERA;
    }

    void AssetViewerWindow::Initialize()
    {
        asset.LoadAsset();
        name = asset.GetName() + "##" + GetID().ToString();
        TaskManagerSystem::Get().RegisterTask(this, &AssetViewerWindow::Render, INT32_MAX - 1, RENDER_HANDLE);

        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER)
        {
            RenderBufferSystem::Get().CreateBuffer(this, std::bind(&AssetViewerWindow::RenderAsset, this), renderBufferWindowResizeCallback);
        }

        // Since I'm setting an equal size for the window will need to calculate the aspect ratio later
        ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f));

        if (windowCustomFlags & WINDOW_SUPPORT_CAMERA)
        {
            camera = new Camera(500.0f, 500.0f);
        }
    }

    void AssetViewerWindow::Uninitialize()
    {
        TaskManagerSystem::Get().RemoveAllTasks(this);
        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER)
        {
            RenderBufferSystem::Get().RemoveBuffer(this);
        }
    }

    void AssetViewerWindow::Focus()
    {
        windowCustomFlags |= WINDOW_FOCUS;
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

        ImGui::Begin(name.c_str(), &isShowing);
        if (windowCustomFlags & WINDOW_FOCUS)
        {
            windowCustomFlags &= ~WINDOW_FOCUS;
            ImGui::SetWindowFocus();
        }

        const ImVec2 currentWindowSize = ImGui::GetContentRegionAvail();
        const int32_t currentX = static_cast<int32_t>(currentWindowSize.x);
        const int32_t currentY = static_cast<int32_t>(currentWindowSize.y);

        if (previousX != currentX || previousY != currentY)
        {
            previousX = currentX;
            previousY = currentY;
            HandleWindowResized();
        }

        RenderContent();
        ImGui::End();
    }

    void AssetViewerWindow::HandleWindowResized()
    {
        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER)
        {
            renderBufferWindowResizeCallback(this, availableX, availableY);
        }

        if (windowCustomFlags & WINDOW_SUPPORT_CAMERA)
        {
            camera->ResizeWindow(availableX, availableY);
        }
    }
}