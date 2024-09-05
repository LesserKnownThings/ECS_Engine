#pragma once

#include "UI/EngineWindow.h"

#include <string>
#include <unordered_map>

namespace LKT
{
	class AssetViewerWindow;

	struct WindowFactoryData;

	/// <summary>
	/// This is the main window for the editor
	/// It will manage all the sub windows like asset viewers and other important windows
	/// </summary>
	class EditorMainWindow : public EngineWindow
	{
	public:
		void Render();
		virtual void Initialize(const std::string &windowName) override;
		virtual void Uninitialize() override;

	protected:
		virtual void RenderContent() override {}

	private:
		EditorMainWindow() = default;

		void ShowDropdown();

		void StartMenuItems(const WindowFactoryData &windowFactoryData);
		void RecursiveMenuItems(std::vector<std::string> &separators, const std::string &windowName);

		friend class UIManager;
	};
}