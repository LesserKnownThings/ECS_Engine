#pragma once

#include "Delegate.h"
#include "UniqueID.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace LKT
{
	class EditorMainWindow;
	class EditorWindow;

	DECLARE_DELEGATE_OneParam(OnWindowClosed, const EditorWindow*)

	struct EditorWindowFactoryData
	{
		std::string windowName;
		std::vector<std::string> subMenus;
		std::function<EditorWindow* ()> factory;
	};

	class UIManager
	{
	public:
		static UIManager& Get();

		~UIManager() = default;

		void InitializeEditorUI();
		void UninitializeEditorUI();

		void RegisterWindow(const EditorWindowFactoryData& factoryData);

		EditorWindow* CreateWindow(const std::string& windowName);	

		OnWindowClosed onWindowClosed;

	private:
		UIManager() = default;

		const std::unordered_map<std::string, EditorWindowFactoryData>& GetFactories() const { return factories; }

		EditorMainWindow* mainWindow = nullptr;

		std::unordered_map<std::string, EditorWindowFactoryData> factories;		

		friend class EditorMainWindow;
	};
}