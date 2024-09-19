#include "ParticleSystemAssetViewer.h"
#include "Assets/ParticleSystemAsset.h"
#include "imgui/imgui.h"
#include "Systems/ParticleSystem/ParticleSystem.h"
#include "Systems/Rendering/RenderBufferObject.h"
#include "Systems/TaskManagerSystem.h"

namespace LKT
{
	ParticleSystemAssetViewer::~ParticleSystemAssetViewer()
	{
		TaskManagerSystem::Get().RemoveAllTasks(this);
	}

	ParticleSystemAssetViewer::ParticleSystemAssetViewer(LazyAssetPtr<Asset> &inAsset)
		: AssetViewerWindow(inAsset)
	{
		if (ParticleSystemAsset *particleSystemAsset = asset.Get<ParticleSystemAsset>())
		{
			ps = particleSystemAsset->GetParticleSystem();
		}

		availableX = 1920;
		availableY = 1080;

		TaskManagerSystem::Get().RegisterTask(this, &ParticleSystemAssetViewer::SimulateAsset, 0, PROCESS_HANDLE);
	}

	void ParticleSystemAssetViewer::RenderContent()
	{
		if (ps != nullptr)
		{
			if (ImGui::BeginTable("ParticleSystem", 2, ImGuiTableFlags_Resizable))
			{
				ImGui::TableSetupColumn("LeftSide");
				ImGui::TableSetupColumn("RightSide");
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);

				ImVec2 availableSize = ImGui::GetContentRegionAvail();

				const uint32_t bufferTex = renderBuffer->GetTextureId();
				if (bufferTex != 0)
				{
					ImTextureID texture = reinterpret_cast<ImTextureID>(bufferTex);
					ImGui::Image((void *)(intptr_t)bufferTex, ImVec2(480, 270), ImVec2(0, 1), ImVec2(1, 0));
				}

				ImGui::TableSetColumnIndex(1);

				ImGui::EndTable();
			}

			static bool resizing = false;

			ImGui::SetCursorPos(ImVec2(7, 35));

			// Render the child window with a border and size
			ImGui::BeginChild("Resizable Child", childSize, true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
			ImGui::Text("This is a resizable child window.");
			ImGui::EndChild();

			ImVec2 cursorPos = ImGui::GetCursorPos();

			// Define a resizing corner
			ImGui::SetCursorPos(ImVec2(cursorPos.x + childSize.x - 15 / 2, cursorPos.y - 10));
			ImGui::Button("##resize_button", ImVec2(15, 15));

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

				if (std::abs(mouseDelta.x) > std::abs(mouseDelta.y))
				{
					childSize.x += mouseDelta.x;
					childSize.y += mouseDelta.x;
				}
				else
				{
					childSize.x += mouseDelta.y;
					childSize.y += mouseDelta.y;
				}

				// Clamp minimum size to avoid collapsing the window
				if (childSize.x < 100)
					childSize.x = 100;
				if (childSize.y < 100)
					childSize.y = 100;
			}
		}
	}

	void ParticleSystemAssetViewer::RenderAssetContent()
	{
		if (ps != nullptr)
		{
			ps->Render();
		}
	}

	void ParticleSystemAssetViewer::SimulateAsset(float deltaTime)
	{
		if (ps != nullptr)
		{
			ps->Simulate(deltaTime);
		}
	}
}