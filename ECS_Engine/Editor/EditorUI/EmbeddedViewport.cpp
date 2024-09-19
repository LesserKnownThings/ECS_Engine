#include "EmbeddedViewport.h"
#include "imgui/imgui.h"

namespace LKT
{
    EmbeddedViewport::~EmbeddedViewport()
    {
    }

    EmbeddedViewport::EmbeddedViewport()
    {

    }

    void EmbeddedViewport::Render()
    {
        const ImVec2 availableScreen = ImGui::GetContentRegionAvail();
    }

    void EmbeddedViewport::HandleWindowResized(int32_t widht, int32_t height)
    {
    }
}