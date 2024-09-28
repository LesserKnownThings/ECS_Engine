#include "InputManagerSystem.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"
#include "Systems/TaskManagerSystem.h"

#include <iostream>

namespace LKT
{
	InputManagerSystem &InputManagerSystem::InputManagerSystem::Get()
	{
		static InputManagerSystem instance;
		return instance;
	}

	InputManagerSystem::~InputManagerSystem()
	{
		TaskManagerSystem::Get().RemoveAllTasks(this);
	}

	InputManagerSystem::InputManagerSystem()
	{
		TaskManagerSystem::Get().RegisterTask(this, &InputManagerSystem::ProcessInput, 0, PROCESS_HANDLE);
	}

	void InputManagerSystem::ProcessInput(float deltaTime)
	{
		RunEvents();

		if (isScrolling)
		{
			currentScrollDelta += deltaTime;
			if (currentScrollDelta >= scrollDelta)
			{
				isScrolling = false;
				currentScrollDelta = 0.f;
				onMouseScroll.Invoke(0.f);
			}
		}

		SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);
	}

	void InputManagerSystem::RunEvents()
	{
		if (isBlocked)
		{
			return;
		}

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_EVENT_QUIT)
			{
				onCloseAppDelegate.Invoke();
			}
			else if (e.type == SDL_EVENT_WINDOW_RESIZED)
			{
				onWindowResized.Invoke();
			}

			if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0)
			{
				mappedKeys[e.key.key] = 1.0f;
			}
			else if (e.type == SDL_EVENT_KEY_UP && e.key.repeat == 0)
			{
				mappedKeys[e.key.key] = 0.0f;
			}

			// These events will be called continously!
			if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (!(e.button.button & pressedMouseButtons))
				{
					onMousePressed.Invoke(e.button);
					pressedMouseButtons &= e.button.button;
				}

				onMouseDown.Invoke(e.button);
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
			{
				onMouseReleased.Invoke(e.button);
				pressedMouseButtons &= ~e.button.button;
			}
			else if (e.type == SDL_EVENT_MOUSE_WHEEL)
			{
				onMouseScroll.Invoke(e.button.x);
				isScrolling = true;
				currentScrollDelta = 0.f;
			}

			ImGui_ImplSDL3_ProcessEvent(&e);
		}
	}

	float InputManagerSystem::GetHorizontalAxis()
	{
		return -mappedKeys[SDLK_A] + mappedKeys[SDLK_D];
	}

	float InputManagerSystem::GetVerticalAxis()
	{
		return mappedKeys[SDLK_W] + -mappedKeys[SDLK_S];
	}
}