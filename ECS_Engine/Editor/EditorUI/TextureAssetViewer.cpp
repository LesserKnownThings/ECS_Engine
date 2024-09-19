#include "TextureAssetViewer.h"
#include "imgui/imgui.h"
#include "Assets/Texture.h"
#include "Systems/TaskManagerSystem.h"

namespace LKT
{
    TextureAssetViewer::TextureAssetViewer(LazyAssetPtr<Asset> &inAsset)
        : AssetViewerWindow(inAsset)
    {
        windowCustomFlags &= ~(WINDOW_SUPPORT_RENDER_BUFFER);
    }

    void TextureAssetViewer::RenderContent()
    {
        if (const Texture *text = asset.Get<Texture>())
        {
            ImVec2 available = ImGui::GetContentRegionAvail();

            const ImVec2 size = ImVec2(text->GetWidth(), text->GetHeight());
            ImVec2 imagePosition = ImVec2((available.x - size.x) * 0.5f, (available.y - size.y) * 0.5f);

            ImVec2 contentRegionMin = ImGui::GetCursorPos();

            ImGui::SetCursorPosX(contentRegionMin.x + imagePosition.x);
            ImGui::SetCursorPosY(contentRegionMin.y + imagePosition.y);

            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(text->GetTextureID())), size);
        }
    }
}