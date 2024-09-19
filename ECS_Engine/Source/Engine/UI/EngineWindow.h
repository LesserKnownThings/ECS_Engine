#pragma once

#include "Delegate.h"
#include "FileHelper.h"
#include "imgui/imgui.h"
#include "Object.h"
#include "UIManager.h"

#include <string>

namespace LKT
{
	constexpr uint8_t WINDOW_FOCUS = 0x001;
	constexpr uint8_t WINDOW_SUPPORT_RENDER_BUFFER = 0x002;

	class EngineWindow : public Object
	{
	public:
		EngineWindow() = default;

		virtual void Initialize(const std::string &windowName);
		virtual void Uninitialize();

		void Render();

		void GetWindowSize(int32_t &x, int32_t &y) const;
		bool IsShowing() const { return isShowing; }
		const std::string &GetName() const { return name; }

	protected:
		virtual void RenderContent() = 0;

		bool isShowing = true;
		bool isFocused = false;
		std::string name;

		uint32_t windowFlags = 0;
		uint8_t windowCustomFlags = 0;
	};

#define REGISTER_WINDOW(type, name)                                                   \
	struct type##Factory                                                              \
	{                                                                                 \
		type##Factory()                                                               \
		{                                                                             \
			std::vector<std::string> separators = FileHelper::SplitString(name, '/'); \
			WindowFactoryData factoryData;                                            \
			if (separators.size() == 0)                                               \
			{                                                                         \
				factoryData.windowName = name;                                        \
			}                                                                         \
			else                                                                      \
			{                                                                         \
				if (separators.size() > 0)                                            \
				{                                                                     \
					factoryData.windowName = separators[separators.size() - 1];       \
					separators.pop_back();                                            \
				}                                                                     \
				if (separators.size() > 0)                                            \
				{                                                                     \
					factoryData.subMenus = separators;                                \
				}                                                                     \
			}                                                                         \
			factoryData.factory = []() { return new type(); };                        \
			UIManager::Get().RegisterWindow(factoryData);                             \
		}                                                                             \
	};                                                                                \
	static type##Factory type##FactoryRegistrar;
}