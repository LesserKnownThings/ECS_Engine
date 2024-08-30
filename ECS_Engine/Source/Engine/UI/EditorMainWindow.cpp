#include "EditorMainWindow.h"
#include "FileHelper.h"
#include "Systems/TaskManagerSystem.h"
#include "UIManager.h"
#include "imgui.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

namespace LKT
{
	namespace
	{
		void SerializeLayoutStream(const std::string& lines)
		{
			if (!fs::exists("Configs"))
			{
				fs::create_directory("Configs");
			}

			std::ofstream file("Configs/layout.ini");

			if (file.is_open())
			{
				file << lines;
				file.close();
			}
		}		
	}

	EditorMainWindow::EditorMainWindow()
	{
		UIManager::Get().onWindowClosed.Bind(this, &EditorMainWindow::HandleWindowClosed);
	}

	void EditorMainWindow::Initialize(const std::string& windowName)
	{
		name = windowName;

		TaskManagerSystem::Get().RegisterTask(this, &EditorMainWindow::Render, INT32_MAX, RENDER_HANDLE);

		Deserialize();
	}

	void EditorMainWindow::Uninitialize()
	{		
		Serialize();

		TaskManagerSystem::Get().RemoveTask(this);
		UIManager::Get().onWindowClosed.Clear(this);

		__super::Uninitialize();
	}

	void EditorMainWindow::Serialize()
	{
		std::ostringstream os;

		for (const auto& kvp : editorWindows)
		{
			os << kvp.second->GetName() << std::endl;
		}

		LKT::SerializeLayoutStream(os.str());
	}

	void EditorMainWindow::Deserialize()
	{
		std::fstream fileString("Configs/layout.ini");

		std::string line;

		if (fileString.is_open())
		{
			while (std::getline(fileString, line))
			{
				if (EditorWindow* newWindow = UIManager::Get().CreateWindow(line))
				{
					editorWindows[line] = newWindow;
				}
			}

			fileString.close();
		}
	}

	void EditorMainWindow::Render()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y));

		ImGui::SetNextWindowBgAlpha(0.0f);

		if (ImGui::Begin(name.c_str(), nullptr, window_flags))
		{
			const ImVec2 window_size = ImGui::GetContentRegionAvail();
			ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, window_size, ImGuiDockNodeFlags_PassthruCentralNode);

			ShowDropdown();
		}

		ImGui::End();
	}	

	void EditorMainWindow::HandleWindowClosed(const EditorWindow* window)
	{
		const std::string currentName = window->GetName();

		if (editorWindows.contains(currentName))
		{
			editorWindows[currentName]->Uninitialize();
			delete editorWindows[currentName];
			editorWindows.erase(currentName);
		}
	}

	void EditorMainWindow::ShowDropdown()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				for (const auto& it : UIManager::Get().GetFactories())
				{
					std::vector<std::string> subMenus = it.second.subMenus;
					RecursiveMenuItems(subMenus, it.second.windowName);
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}

	void EditorMainWindow::RecursiveMenuItems(std::vector<std::string>& separators, const std::string& windowName)
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
					if (!editorWindows.contains(windowName))
					{
						editorWindows[windowName] = UIManager::Get().CreateWindow(windowName);
					}
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