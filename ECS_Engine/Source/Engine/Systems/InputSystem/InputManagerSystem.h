#pragma once

#include "Delegate.h"
#include "Math/Numerals.h"
#include "glm/glm.hpp"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "UniqueID.h"

#include <unordered_map>

namespace LKT
{
	DECLARE_DELEGATE(OnCloseApp)
	DECLARE_DELEGATE(OnWindowResized)
	DECLARE_DELEGATE_TwoParams(OnWindowResizedParams, int32_t, int32_t)

	DECLARE_DELEGATE_OneParam(MouseActionDelegate, const SDL_MouseButtonEvent&)

	class InputManagerSystem
	{
	public:
		static InputManagerSystem& Get();

		~InputManagerSystem();

		float GetHorizontalAxis();
		float GetVerticalAxis();

		const glm::ivec2& GetMouseDelta() const { return mouseDelta; }

		OnCloseApp onCloseAppDelegate;
		OnWindowResized onWindowResized;
		OnWindowResizedParams onWindowResizedParams;

		MouseActionDelegate onMousePressed;
		MouseActionDelegate onMouseReleased;

		//For continous mouse press
		MouseActionDelegate onMouseDown;

	private:
		InputManagerSystem();

		void ProcessInput(float deltaTime);
		void RunEvents();

		std::unordered_map<SDL_Keycode, float> mappedKeys;
		glm::ivec2 mouseDelta;

		uint32_t pressedMouseButtons = 0;
	};
}