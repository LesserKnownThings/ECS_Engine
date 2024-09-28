#include "AssetViewerWindow.h"
#include "Camera.h"
#include "Engine.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/ImGuizmo.h"
#include "SDL/SDL.h"
#include "Systems/AssetManager/Asset.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "Systems/Rendering/RenderBufferObject.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

namespace LKT
{
    AssetViewerWindow::AssetViewerWindow(LazyAssetPtr<Asset> &inAsset)
        : asset(inAsset)
    {
        windowCustomFlags = WINDOW_SUPPORT_RENDER_BUFFER;
    }

    void AssetViewerWindow::Initialize()
    {
        asset.LoadAsset();
        name = asset.GetName() + "##" + GetID().ToString();
        TaskManagerSystem::Get().RegisterTask(this, &AssetViewerWindow::Render, INT32_MAX, RENDER_HANDLE);

        // Since I'm setting an equal size for the window will need to calculate the aspect ratio later
        ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f));

        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER)
        {
            renderBuffer = new RenderBufferObject(500, 500);
            camera = new Camera(500, 500);
            InputManagerSystem::Get().onMouseScroll.Bind(this, &AssetViewerWindow::HandleZoom);
            InputManagerSystem::Get().onMousePressed.Bind(this, &AssetViewerWindow::HandleMousePressed);
            InputManagerSystem::Get().onMouseReleased.Bind(this, &AssetViewerWindow::HandleMouseReleased);

            TaskManagerSystem::Get().RegisterTask(this, &AssetViewerWindow::ProcessMouseMovement, 0, PROCESS_HANDLE);
            TaskManagerSystem::Get().RegisterTask(this, &AssetViewerWindow::RenderAsset, 0, RENDER_HANDLE);
        }
    }

    void AssetViewerWindow::Uninitialize()
    {
        // This also holds the window rendering so I need to remove the tasks even without buffer support
        TaskManagerSystem::Get().RemoveAllTasks(this);

        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER)
        {
            delete renderBuffer;
            delete camera;
            InputManagerSystem::Get().onMouseScroll.Clear(this);
            InputManagerSystem::Get().onMousePressed.Clear(this);
            InputManagerSystem::Get().onMouseReleased.Clear(this);
        }
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

        ImGui::Begin(name.c_str(), &isShowing, windowFlags);
        if (windowCustomFlags & WINDOW_FOCUS)
        {
            windowCustomFlags &= ~WINDOW_FOCUS;
            ImGui::SetWindowFocus();
        }

        isFocused = ImGui::IsWindowFocused();

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

    void AssetViewerWindow::ProcessMouseMovement(float deltaTime)
    {
        if (camera != nullptr && isMouseMoving)
        {
            const glm::vec2 &mouseDelta = InputManagerSystem::Get().GetMouseDelta();
            // assetModel = glm::rotate(assetModel, glm::radians(deltaTime * mouseDelta.x * 15.0f), glm::vec3(0.f, 1.f, 0.f));
            //   assetModel = glm::rotate(assetModel, glm::radians(deltaTime * mouseDelta.y * 15.0f), glm::vec3(1.f, 0.f, 0.f));
        }

        ShaderManager::Get().SetMat4f("model", assetModel);
    }

    void AssetViewerWindow::HandleWindowResized()
    {
        if (windowCustomFlags & WINDOW_SUPPORT_RENDER_BUFFER && renderBuffer != nullptr)
        {
            renderBuffer->HandleSizeChanged(availableX, availableY);
            if (camera != nullptr)
            {
                camera->ResizeWindow(availableX, availableY);
            }
        }
    }

    void AssetViewerWindow::HandleZoom(float amount)
    {
        if (isFocused && camera != nullptr)
        {
            camera->Zoom(amount * EngineInstance->GetDeltaTime() * 50.0f);
        }
    }

    void AssetViewerWindow::HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_MIDDLE)
        {
            isMouseMoving = true;
        }
    }

    void AssetViewerWindow::HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_MIDDLE)
        {
            isMouseMoving = false;
        }
    }

    void AssetViewerWindow::RenderAsset()
    {
        if (renderBuffer != nullptr)
        {
            renderBuffer->Use();
            camera->Present();
            RenderAssetContent();
            renderBuffer->Dispose();
        }
    }
}