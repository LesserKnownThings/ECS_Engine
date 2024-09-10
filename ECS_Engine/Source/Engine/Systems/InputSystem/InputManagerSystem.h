#pragma once

#include "Delegate.h"
#include "glm/glm.hpp"
#include "SDL/SDL_events.h"
#include "SDL/SDL_keycode.h"
#include "SDL/SDL_mouse.h"
#include "UniqueID.h"

#include <cstdint>
#include <unordered_map>

namespace LKT
{
	DECLARE_DELEGATE(OnCloseApp);
	DECLARE_DELEGATE(OnWindowResized);

	DECLARE_DELEGATE_OneParam(MouseActionDelegate, const SDL_MouseButtonEvent &);

	class InputManagerSystem
	{
	public:
		static InputManagerSystem &Get();

		~InputManagerSystem();

		float GetHorizontalAxis();
		float GetVerticalAxis();

		void BlockInput() { isBlocked = true; }
		void UnblockInput() { isBlocked = false; }

		const glm::vec2 &GetMouseDelta() const { return mouseDelta; }

		OnCloseApp onCloseAppDelegate;
		OnWindowResized onWindowResized;

		MouseActionDelegate onMousePressed;
		MouseActionDelegate onMouseReleased;

		// For continous mouse press
		MouseActionDelegate onMouseDown;

	private:
		InputManagerSystem();

		void ProcessInput(float deltaTime);
		void RunEvents();

		std::unordered_map<SDL_Keycode, float> mappedKeys;
		glm::vec2 mouseDelta;

		uint32_t pressedMouseButtons = 0;
		bool isBlocked = false;
	};
}