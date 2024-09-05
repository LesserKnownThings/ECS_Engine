#include "TextureAssetViewer.h"
#include "imgui/imgui.h"
#include "Assets/Texture.h"

namespace LKT
{
    TextureAssetViewer::TextureAssetViewer(LazyAssetPtr<Asset> &inAsset)
        : AssetViewerWindow(inAsset)
    {
    }

    void TextureAssetViewer::RenderContent()
    {
        if (const Texture *text = asset.Get<Texture>())
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(text->GetTextureID())), ImVec2(text->GetWidth(), text->GetHeight()));
        }
    }
}