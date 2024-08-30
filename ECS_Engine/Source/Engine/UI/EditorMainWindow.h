#pragma once

#include "EditorWindow.h"

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace LKT
{
	/// <summary>
	/// This is the main window for the editor
	/// It will manage all the sub windows like asset viewers and other important windows
	/// </summary>
	class EditorMainWindow : public EditorWindow
	{
	public:
		void Render();
		virtual void Initialize(const std::string& windowName) override;
		virtual void Uninitialize() override;

		//TODO this is temporary to keep the state of the editor, will move this to a build tool, or something more complex
		void Serialize();
		void Deserialize();

	protected:
		virtual void RenderContent() override {}

	private:
		EditorMainWindow();

		void HandleWindowClosed(const EditorWindow* window);
		void ShowDropdown();
		void RecursiveMenuItems(std::vector<std::string>& separators, const std::string& windowName);

		std::unordered_map<std::string, EditorWindow*> editorWindows;

		friend class UIManager;
	};
}