#pragma once

#include "BuildMacros.h"
#include "Delegate.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace LKT
{
#if EDITOR_ONLY
	class EditorMainWindow;
#endif

	class EngineWindow;
	class AssetViewerWindow;

	struct AssetPath;

	DECLARE_DELEGATE_OneParam(OnWindowClosed, const EngineWindow *);
	DECLARE_DELEGATE_OneParam(OnUniqueWindowClosed, const AssetViewerWindow *);

	struct WindowFactoryData
	{
		std::string windowName;
		std::vector<std::string> subMenus;
		std::function<EngineWindow *()> factory;
	};

	class UIManager
	{
	public:
		static UIManager &Get();

		~UIManager() = default;

		void InitializeUI();
		void UninitializeUI();

		EngineWindow *GetWindow(const std::string &windowName);

#if EDITOR_ONLY
		void RegisterWindow(const WindowFactoryData &factoryData);
		void RequestAssetViewer(const AssetPath &assetPath);
#endif

		OnWindowClosed onWindowClosed;
		OnUniqueWindowClosed onUniqueWindowClosed;

	private:
		UIManager() = default;

#if EDITOR_ONLY
		void Serialize();
		void Deserialize();
#endif

		void HandleUniqueWindowClosed(const AssetViewerWindow *window);
		void HandleWindowClosed(const EngineWindow *window);

		const std::unordered_map<std::string, WindowFactoryData> &GetFactories() const { return factories; }

		std::unordered_map<std::string, WindowFactoryData> factories;

		std::unordered_map<std::string, EngineWindow *> windows;
		std::unordered_map<std::string, AssetViewerWindow *> uniqueWindows;

#if EDITOR_ONLY
		EditorMainWindow *mainWindow = nullptr;

		friend class EditorMainWindow;
#endif
	};
}