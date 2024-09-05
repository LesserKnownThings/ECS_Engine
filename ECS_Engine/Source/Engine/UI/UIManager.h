#pragma once

#include "BuildMacros.h"
#include "Delegate.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "UniqueID.h"

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

	DECLARE_DELEGATE_OneParam(OnWindowClosed, const EngineWindow *)
		DECLARE_DELEGATE_OneParam(OnUniqueWindowClosed, const EngineWindow *)

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

		void RegisterWindow(const WindowFactoryData &factoryData);

		EngineWindow *GetWindow(const std::string &windowName);
		EngineWindow *GetUniqueWindow(const std::string &windowName);

		void RequestAssetViewer(const std::filesystem::path &assetPath);

		OnWindowClosed onWindowClosed;
		OnUniqueWindowClosed onUniqueWindowClosed;

	private:
		UIManager() = default;

#if EDITOR_ONLY
		void Serialize();
		void Deserialize();
#endif

		void HandleUniqueWindowClosed(const EngineWindow *window);
		void HandleWindowClosed(const EngineWindow *window);

		const std::unordered_map<std::string, WindowFactoryData> &GetFactories() const { return factories; }

		std::unordered_map<std::string, WindowFactoryData> factories;

		std::unordered_map<std::string, EngineWindow *> windows;
		std::unordered_map<std::string, EngineWindow *> uniqueWindows;

#if EDITOR_ONLY
		EditorMainWindow *mainWindow = nullptr;

		friend class EditorMainWindow;
#endif
	};
}