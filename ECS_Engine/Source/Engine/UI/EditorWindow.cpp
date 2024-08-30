#include "EditorWindow.h"
#include "imgui.h"
#include "Systems/TaskManagerSystem.h"
#include "UIManager.h"

namespace LKT
{
	void EditorWindow::Initialize(const std::string& windowName)
	{
		name = windowName;

		TaskManagerSystem::Get().RegisterTask(this, &EditorWindow::Render, INT32_MAX - 1, RENDER_HANDLE);
	}

	void EditorWindow::Uninitialize()
	{
		TaskManagerSystem::Get().RemoveTask(this);
	}

	void EditorWindow::Render()
	{
		if (isShowing)
		{
			if (ImGui::Begin(name.c_str(), &isShowing))
			{
				RenderContent();
			}

			ImGui::End();
		}
		else
		{
			UIManager::Get().onWindowClosed.Invoke(this);
		}
	}
}