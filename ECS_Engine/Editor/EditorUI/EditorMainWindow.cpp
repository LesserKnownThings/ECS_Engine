#include "EditorMainWindow.h"
#include "AssetViewerWindow.h"
#include "imgui/imgui.h"
#include "Systems/TaskManagerSystem.h"
#include "UI/UIManager.h"

#include <iostream>
#include <vector>

namespace LKT
{
	void EditorMainWindow::Initialize(const std::string &windowName)
	{
		name = windowName;

		TaskManagerSystem::Get().RegisterTask(this, &EditorMainWindow::Render, INT32_MAX, RENDER_HANDLE);

		windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	}

	void EditorMainWindow::Uninitialize()
	{
		TaskManagerSystem::Get().RemoveAllTasks(this);
		EngineWindow::Uninitialize();
	}

	void EditorMainWindow::Render()
	{
		const ImGuiViewport *main_viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y));

		ImGui::SetNextWindowBgAlpha(0.0f);

		ImGui::Begin(name.c_str(), nullptr, windowFlags);
		const ImVec2 window_size = ImGui::GetContentRegionAvail();
		ImGuiID dockspaceID = ImGui::GetID("MainWindowDockSpace");
		ImGui::DockSpace(dockspaceID, window_size, ImGuiDockNodeFlags_PassthruCentralNode);

		ShowDropdown();
		ImGui::End();
	}

	void EditorMainWindow::ShowDropdown()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				for (const auto &it : UIManager::Get().GetFactories())
				{
					StartMenuItems(it.second);
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}

	void EditorMainWindow::StartMenuItems(const WindowFactoryData &windowFactoryData)
	{
		if (windowFactoryData.subMenus.size() > 0)
		{
			std::vector<std::string> separators = windowFactoryData.subMenus;
			RecursiveMenuItems(separators, windowFactoryData.windowName);
		}
		else
		{
			if (ImGui::MenuItem(windowFactoryData.windowName.c_str()))
			{
				UIManager::Get().GetWindow(windowFactoryData.windowName);
			}
		}
	}

	void EditorMainWindow::RecursiveMenuItems(std::vector<std::string> &separators, const std::string &windowName)
	{
		if (separators.size() == 0)
		{
			return;
		}

		if (ImGui::BeginMenu(separators[0].c_str()))
		{
			if (separators.size() == 1)
			{
				if (ImGui::MenuItem(windowName.c_str()))
				{
					UIManager::Get().GetWindow(windowName);
				}
			}
			else
			{
				separators.erase(separators.begin());
				RecursiveMenuItems(separators, windowName);
			}

			ImGui::EndMenu();
		}
	}
}