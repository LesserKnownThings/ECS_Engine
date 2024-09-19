#include "UIManager.h"
#include "EngineWindow.h"
#include "Systems/TaskManagerSystem.h"

#if EDITOR_ONLY
#include "EditorUI/AssetViewerWindow.h"
#include "EditorUI/EditorMainWindow.h"
#include "Systems/AssetManager/AssetFactory/AssetFactoryManager.h"
#include "Systems/AssetManager/AssetManager.h"
#include "Systems/AssetManager/AssetPath.h"
#endif

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace LKT
{
	namespace
	{
		void SerializeLayoutStream(const std::string &lines)
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

	UIManager &UIManager::Get()
	{
		static UIManager instance;
		return instance;
	}

	void UIManager::InitializeUI()
	{
#if EDITOR_ONLY
		mainWindow = new EditorMainWindow();
		mainWindow->Initialize("LKT ECS Engine");
#endif

		onWindowClosed.Bind(this, &UIManager::HandleWindowClosed);
		onUniqueWindowClosed.Bind(this, &UIManager::HandleUniqueWindowClosed);

		Deserialize();
	}

	void UIManager::UninitializeUI()
	{
		Serialize();

		onWindowClosed.Clear(this);
		onUniqueWindowClosed.Clear(this);

#if EDITOR_ONLY
		if (mainWindow != nullptr)
		{
			mainWindow->Uninitialize();
			delete mainWindow;
			mainWindow = nullptr;
		}
#endif
	}

	EngineWindow *UIManager::GetWindow(const std::string &windowName)
	{
		const auto windowIt = windows.find(windowName);

		if (windowIt != windows.end())
		{
			return windowIt->second;
		}

		const auto it = factories.find(windowName);

		if (it != factories.end())
		{
			EngineWindow *newWindow = it->second.factory();

			if (newWindow != nullptr)
			{
				newWindow->Initialize(it->second.windowName);
				windows[windowName] = newWindow;

				return newWindow;
			}
		}

		return nullptr;
	}

#if EDITOR_ONLY
	void UIManager::RegisterWindow(const WindowFactoryData &factoryData)
	{
		factories[factoryData.windowName] = factoryData;
	}

	void UIManager::RequestAssetViewer(const AssetPath &assetPath)
	{
		const auto it = windows.find(assetPath.fullPath);
		if (it != windows.end())
		{
			// TODO will probably remove this, since it's not very useful
			// it->second->Focus();
			return;
		}

		if (AssetViewerWindow *assetViewer = AssetManager::LoadAssetViewer(assetPath))
		{
			windows[assetPath.fullPath] = assetViewer;
		}
	}

	void UIManager::Serialize()
	{
		std::ostringstream os;

		for (const auto &kvp : windows)
		{
			os << kvp.second->GetName() << std::endl;
		}

		LKT::SerializeLayoutStream(os.str());
	}

	void UIManager::Deserialize()
	{
		std::fstream fileString("Configs/layout.ini");

		std::string line;

		if (fileString.is_open())
		{
			while (std::getline(fileString, line))
			{
				GetWindow(line);
			}

			fileString.close();
		}
	}
#endif

	// TODO maybe I should add a task for the GC instead of instantly removing these windows
	void UIManager::HandleUniqueWindowClosed(const AssetViewerWindow *window)
	{
		if (window != nullptr)
		{
			const auto it = windows.find(window->GetAssetPath().fullPath);

			if (it != windows.end())
			{
				it->second->Uninitialize();
				delete it->second;
				windows.erase(it);
			}
		}
	}

	void UIManager::HandleWindowClosed(const EngineWindow *window)
	{
		if (window != nullptr)
		{
			const auto it = windows.find(window->GetName());

			if (it != windows.end())
			{
				it->second->Uninitialize();
				delete it->second;
				windows.erase(it);
			}
		}
	}
}
