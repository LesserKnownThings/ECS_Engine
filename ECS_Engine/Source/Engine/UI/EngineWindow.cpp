#include "EngineWindow.h"
#include "imgui/imgui.h"
#include "Systems/TaskManagerSystem.h"
#include "UIManager.h"

namespace LKT
{
	void EngineWindow::Initialize(const std::string& windowName)
	{
		name = windowName;

		TaskManagerSystem::Get().RegisterTask(this, &EngineWindow::Render, INT32_MAX - 1, RENDER_HANDLE);
	}

	void EngineWindow::Uninitialize()
	{
		TaskManagerSystem::Get().RemoveTask(this);
	}

	void EngineWindow::Render()
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