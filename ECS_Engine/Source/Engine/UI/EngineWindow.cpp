#include "EngineWindow.h"
#include "Systems/TaskManagerSystem.h"
#include "UIManager.h"

namespace LKT
{
	void EngineWindow::Initialize(const std::string &windowName)
	{
		name = windowName;

		TaskManagerSystem::Get().RegisterTask(this, &EngineWindow::Render, INT32_MAX, RENDER_HANDLE);
	}

	void EngineWindow::Uninitialize()
	{
		TaskManagerSystem::Get().RemoveAllTasks(this);
	}

	void EngineWindow::Render()
	{
		if (isShowing)
		{
			ImGui::Begin(name.c_str(), &isShowing, windowFlags);
			isFocused = ImGui::IsWindowFocused();
			RenderContent();
			ImGui::End();
		}
		else
		{
			UIManager::Get().onWindowClosed.Invoke(this);
		}
	}

	void EngineWindow::GetWindowSize(int32_t &x, int32_t &y) const
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		x = windowSize.x;
		y = windowSize.y;
	}
}