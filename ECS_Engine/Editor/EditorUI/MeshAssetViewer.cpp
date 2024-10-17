#include "MeshAssetViewer.h"
#include "Assets/Mesh.h"
#include "glew/glew.h"
#include "Components/RenderComponent.h"
#include "Systems/Rendering/RenderBufferObject.h"
#include "Systems/ShaderSystem/ShaderManager.h"

namespace LKT
{
    MeshAssetViewer::MeshAssetViewer(LazyAssetPtr<Asset> &inAsset)
        : AssetViewerWindow(inAsset)
    {
        availableX = 500.0f;
        availableY = 500.0f / ASSET_VIEWER_ASPECT_RATIO;
    }

    void MeshAssetViewer::RenderContent()
    {
        const uint32_t bufferTex = renderBuffer->GetTextureId();
        if (bufferTex != 0)
        {
            ImVec2 available = ImGui::GetContentRegionAvail();

            availableX = available.x;
            availableY = available.x / ASSET_VIEWER_ASPECT_RATIO;

            if (availableY > available.y)
            {
                availableY = available.y;
                availableX = available.y * ASSET_VIEWER_ASPECT_RATIO;
            }

            float offsetX = (available.x - availableX) / 2.0f;
            float offsetY = (available.y - availableY) / 2.0f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

            ImTextureID texture = reinterpret_cast<ImTextureID>(bufferTex);
            ImGui::Image((void *)(intptr_t)bufferTex, ImVec2(availableX, availableY), ImVec2(0, 1), ImVec2(1, 0));
        }
    }

    void MeshAssetViewer::RenderAssetContent()
    {
        if (const Mesh *mesh = asset.Get<Mesh>())
        {
            const RenderComponentObject &rco = mesh->GetRenderComponentObject();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rco.textureID);

            glBindVertexArray(rco.vao);
            glDrawElements(GL_TRIANGLES, rco.elementCount, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
        }
    }
}