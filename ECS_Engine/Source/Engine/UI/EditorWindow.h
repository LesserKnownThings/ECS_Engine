#pragma once

#include "FileHelper.h"
#include "Object.h"
#include "UIManager.h"

#include <string>
#include "typeindex"

namespace LKT
{
	class EditorWindow : public Object
	{
	public:
		EditorWindow() = default;

		virtual void Initialize(const std::string& windowName);
		virtual void Uninitialize();

		void Render();

		bool IsShowing() const { return isShowing; }
		const std::string& GetName() const { return name; }

	protected:
		virtual void RenderContent() = 0;

		bool isShowing = true;
		std::string name;
	};

#define REGISTER_WINDOW(type, name) \
	struct type##Factory \
	{ \
		type##Factory() \
		{\
			std::vector<std::string> separators = FileHelper::SplitString(name, '/'); \
			EditorWindowFactoryData factoryData; \
			if(separators.size() > 0) \
			{ \
				factoryData.windowName = separators[separators.size() - 1]; \
				separators.pop_back(); \
			} \
			if(separators.size() > 0) \
			{ \
				factoryData.subMenus = separators; \
			} \
			factoryData.factory = []() { return new type(); }; \
			UIManager::Get().RegisterWindow(factoryData); \
		} \
	}; \
	static type##Factory type##FactoryRegistrar;
}