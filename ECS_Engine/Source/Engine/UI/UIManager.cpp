#include "UIManager.h"
#include "EditorMainWindow.h"
#include "EditorWindow.h"
#include "Systems/TaskManagerSystem.h"

namespace LKT
{
	UIManager& UIManager::Get()
	{
		static UIManager instance;
		return instance;
	}

	void UIManager::InitializeEditorUI()
	{
		mainWindow = new EditorMainWindow();
		mainWindow->Initialize("LKT ECS Engine");
	}

	void UIManager::UninitializeEditorUI()
	{
		if (mainWindow != nullptr)
		{
			mainWindow->Uninitialize();
			delete mainWindow;
			mainWindow = nullptr;
		}
	}

	void UIManager::RegisterWindow(const EditorWindowFactoryData& factoryData)
	{
		factories[factoryData.windowName] = factoryData;
	}

	EditorWindow* UIManager::CreateWindow(const std::string& windowName)
	{
		const auto it = factories.find(windowName);

		if (it != factories.end())
		{
			EditorWindow* newWindow = it->second.factory();

			newWindow->Initialize(it->second.windowName);

			return newWindow;
		}

		return nullptr;
	}
}
